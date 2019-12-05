#include "node.h"
#include "utility.h"
#include <kdbassert.h>
#include <kdbhelper.h>
#include <kdbmeta.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Node * createNode (Key * key, Node * parent);
static bool addChild (Node * parent, Node * child);
static Node * buildTreeTableArray (Node * parent, Key * root, KeySet * keys);
static Node * buildTreeArray (Node * parent, Key * root, KeySet * keys);
static void sortChildren (Node * node);
static int nodeCmpWrapper (const void * a, const void * b);
static NodeType getNodeType (Key * key);
static char * getRelativeName (Key * parent, Key * key);

Node * buildTree (Node * parent, Key * root, KeySet * keys)
{
	if (isTableArray (root))
	{
		return buildTreeTableArray (parent, root, keys);
	}
	else if (isArray (root))
	{
		return buildTreeArray (parent, root, keys);
	}
	else
	{
		Node * node = createNode (root, parent);
		Key * key;

		if (node->type != NT_LEAF)
		{
			while ((key = ksCurrent (keys)) != NULL && keyIsBelow (root, key) == 1)
			{
				ksNext (keys);
				Node * child = buildTree (node, key, keys);
				// printf ("<< BUILT TREE, back at %s\n", keyName (node->key));
				if (child == NULL)
				{
					destroyTree (node);
					return NULL;
				}
				if (!addChild (node, child))
				{
					destroyTree (node);
					NULL;
				}
			}
			sortChildren (node);
		}
		return node;
	}
}

static Node * buildTreeTableArray (Node * parent, Key * root, KeySet * keys)
{
	ELEKTRA_ASSERT (isTableArray (root), "Root must be a table array, but wasn't");
	Node * node = createNode (root, parent);

	size_t max = getArrayMax (root);
	for (size_t i = 0; i <= max; i++)
	{
		Key * elementName = keyAppendIndex (i, root);
		Node * element = buildTree (node, elementName, keys);
		if (!addChild (node, element))
		{
			destroyTree (node);
			return NULL;
		}
	}
	return node;
}

static Node * buildTreeArray (Node * parent, Key * root, KeySet * keys)
{
	ELEKTRA_ASSERT (isArray (root) && !isTableArray (root), "Root must be array, but no table array, but wasn't");
	Node * node = createNode (root, parent);

	size_t max = getArrayMax (root);
	for (size_t i = 0; i <= max; i++)
	{
		Key * elementName = keyAppendIndex (i, root);
		Key * elementKey = ksLookup (keys, elementName, 0);
		// TODO: handle array holes
		if (elementKey != NULL)
		{
			if (!addChild (node, buildTree (node, elementKey, keys)))
			{
				destroyTree (node);
				return NULL;
			}
		}
	}
	Key * key;
	while ((key = ksCurrent (keys)) != NULL && keyIsBelow (root, key) == 1)
	{
		ksNext (keys);
	}
	return node;
}

void destroyTree (Node * node)
{
	if (node != NULL)
	{
		if (node->relativeName != NULL)
		{
			elektraFree (node->relativeName);
		}
		for (size_t i = 0; i < node->childCount; i++)
		{
			destroyTree (node->children[i]);
		}
		elektraFree (node->children);
		elektraFree (node);
	}
}

static Node * createNode (Key * key, Node * parent)
{
	Node * node = (Node *) elektraCalloc (sizeof (Node));
	if (node == NULL)
	{
		return NULL;
	}
	node->key = key;
	node->parent = parent;
	if (parent == NULL)
	{
		node->relativeName = NULL; // elektraStrDup ("");
		node->type = NT_ROOT;
	}
	else
	{
		node->type = getNodeType (key);
		node->relativeName = getRelativeName (parent->key, key);
		if (node->relativeName == NULL)
		{
			destroyTree (node);
			return NULL;
		}
		if (node->type == NT_SIMPLE_TABLE || node->type == NT_TABLE_ARRAY)
		{
			Node * tableAncestor = node;
			while ((tableAncestor = tableAncestor->parent) != NULL)
			{
				if (tableAncestor->type == NT_SIMPLE_TABLE || tableAncestor->type == NT_TABLE_ARRAY)
				{
					if (!(node->type == NT_SIMPLE_TABLE && tableAncestor->type == NT_TABLE_ARRAY))
					{
						size_t split = elektraStrLen (tableAncestor->relativeName) - 1;
						size_t len = split + elektraStrLen (node->relativeName) + 1;
						char * concatName = (char *) elektraCalloc (len);
						if (concatName == NULL)
						{
							destroyTree (node);
							return NULL;
						}
						strncpy (concatName, tableAncestor->relativeName, len);
						concatName[split] = '.';
						strncat (&concatName[split + 1], node->relativeName, len - split - 1);
						elektraFree (node->relativeName);
						node->relativeName = concatName;
						break;
					}
				}
			}
		}

		/*printf (">> CREATE NODE\nparent =\t%s\nkey =\t\t%s\nrelative =\t%s\ntype =\t\t%d\n",
			parent != NULL ? keyName (parent->key) : "<NONE>", keyName (key), node->relativeName, node->type);*/
	}
	return node;
}

static bool addChild (Node * parent, Node * child)
{
	if (parent->childSize == 0)
	{
		parent->childSize = 4;
		parent->children = (Node **) elektraCalloc (sizeof (Node *) * parent->childSize);
		if (parent->children == NULL)
		{
			destroyTree (parent);
			return NULL;
		}
	}
	else if (parent->childCount == parent->childSize)
	{
		parent->childSize *= 2;
		if (elektraRealloc ((void **) &parent->children, sizeof (Node *) * parent->childSize) < 0)
		{
			parent->childSize /= 2;
			return false;
		}
	}
	parent->children[parent->childCount++] = child;
	return true;
}

static void sortChildren (Node * node)
{
	if (node->childCount > 1)
	{
		qsort (node->children, node->childCount, sizeof (Node *), nodeCmpWrapper);
	}
}

static int nodeCmpWrapper (const void * a, const void * b)
{
	return elektraKeyCmpOrder (*(const Key **) ((const Node *) a)->key, *(const Key **) ((const Node *) b)->key);
}

static NodeType getNodeType (Key * key)
{
	if (isArray (key))
	{
		if (isTableArray (key))
		{
			return NT_TABLE_ARRAY;
		}
		else
		{
			return NT_ARRAY;
		}
	}
	else if (isSimpleTable (key))
	{
		return NT_SIMPLE_TABLE;
	}
	else if (isInlineTable (key))
	{
		return NT_INLINE_TABLE;
	}
	else if (isArrayIndex (keyBaseName (key)))
	{
		return NT_LIST_ELEMENT;
	}
	else
	{
		return NT_LEAF;
	}
}

static char * getRelativeName (Key * parent, Key * key)
{
	size_t nameSize = 64;
	size_t pos = 0;
	char * name = (char *) elektraCalloc (sizeof (char) * nameSize);
	bool placeDot = false;
	const char * keyPart = ((const char *) keyUnescapedName (key)) + keyGetUnescapedNameSize (parent);
	const char * keyStop = ((const char *) keyUnescapedName (key)) + keyGetUnescapedNameSize (key);

	if (isTableArray (parent))
	{ // skip array index
		keyPart += elektraStrLen (keyPart);
	}
	while (keyPart < keyStop)
	{
		if (placeDot)
		{
			if (pos == nameSize)
			{
				nameSize *= 2;
				if (elektraRealloc ((void **) &name, nameSize) < 0)
				{
					elektraFree (name);
					return NULL;
				}
			}
			name[pos++] = '.';
		}
		else
		{
			placeDot = true;
		}
		bool bare = isBareString (keyPart);
		if (!bare)
		{
			if (pos == nameSize)
			{
				nameSize *= 2;
				if (elektraRealloc ((void **) &name, nameSize) < 0)
				{
					elektraFree (name);
					return NULL;
				}
			}
			name[pos++] = '"';
		}
		if (pos + elektraStrLen (keyPart) >= nameSize)
		{
			nameSize *= 2;
			if (elektraRealloc ((void **) &name, nameSize) < 0)
			{
				elektraFree (name);
				return NULL;
			}
		}
		strncat (&name[pos], keyPart, nameSize - pos);
		pos += elektraStrLen (keyPart) - 1;
		if (!bare)
		{
			if (pos == nameSize)
			{
				nameSize *= 2;
				if (elektraRealloc ((void **) &name, nameSize) < 0)
				{
					elektraFree (name);
					return NULL;
				}
			}
			name[pos++] = '"';
		}
		keyPart += elektraStrLen (keyPart);
	}
	name[pos] = 0;
	return name;
}

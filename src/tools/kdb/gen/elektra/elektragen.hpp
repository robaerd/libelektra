/**
 * @file
 *
 * @brief
 *
 * @copyright BSD License (see LICENSE.md or https://www.libelektra.org)
 */

#ifndef ELEKTRA_ELEKTRAGEN_HPP
#define ELEKTRA_ELEKTRAGEN_HPP

#include "../template.hpp"

class ElektraGenTemplate : public GenTemplate
{
	struct Params
	{
		static const char * InitFunctionName;
		static const char * HelpFunctionName;
		static const char * SpecloadFunctionName;
		static const char * TagPrefix;
		static const char * EnumConversion;
		static const char * AdditionalHeaders;
		static const char * GenerateSetters;
		static const char * SpecLocation;
		static const char * DefaultsHandling;
		static const char * SpecValidation;
	};

public:
	ElektraGenTemplate ()
	: GenTemplate ("elektra", { ".c", ".h" },
		       { "enum.c", "union.c", "struct.c", "struct.alloc.fields.c", "enum.decl.h", "struct.decl.h", "union.decl.h",
			 "keys.fun.h", "keys.fun.struct.h", "keys.fun.structref.h", "keys.tags.h", "context.fun.h", "context.tags.h" },
		       {
			       { Params::InitFunctionName, false },
			       { Params::HelpFunctionName, false },
			       { Params::SpecloadFunctionName, false },
			       { Params::TagPrefix, false },
			       { Params::EnumConversion, false },
			       { Params::GenerateSetters, false },
			       { Params::AdditionalHeaders, false },
			       { Params::SpecLocation, false },
			       { Params::DefaultsHandling, false },
			       { Params::SpecValidation, false },
		       })
	{
	}

protected:
	kainjow::mustache::data getTemplateData (const std::string & outputName, const std::string & part, const kdb::KeySet & ks,
						 const std::string & parentKey) const override;

	std::string escapeFunction (const std::string & str) const override;
};

#endif // ELEKTRA_ELEKTRAGEN_HPP

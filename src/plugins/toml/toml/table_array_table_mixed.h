// clang-format off
ksNew (16,

	keyNew (PREFIX, KEY_VALUE, "@CONFIG_FILEPATH@", KEY_END),

	keyNew (PREFIX "/table_array",
		KEY_META, "array", "#1",
		KEY_META, "type", "tablearray",
		KEY_META, "order", "0",
	KEY_END),

	keyNew (PREFIX "/table_array/#0/a",
		KEY_VALUE, "0",
		KEY_META, "order", "1",
	KEY_END),
	keyNew (PREFIX "/table_array/#0/table",
		KEY_META, "type", "simpletable",
		KEY_META, "order", "2",
	KEY_END),
	keyNew (PREFIX "/table_array/#0/table/b",
		KEY_VALUE, "1",
		KEY_META, "order", "3",
	KEY_END),

	keyNew (PREFIX "/table_array/#1/a",
		KEY_VALUE, "10",
		KEY_META, "order", "4",
	KEY_END),
	keyNew (PREFIX "/table_array/#1/table",
		KEY_META, "type", "simpletable",
		KEY_META, "order", "5",
	KEY_END),
	keyNew (PREFIX "/table_array/#1/table/b",
		KEY_VALUE, "20",
		KEY_META, "order", "6",
	KEY_END),

KS_END)

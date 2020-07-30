#ifndef RS_COMMANDS_H_
#define RS_COMMANDS_H_

/** RS_CMD_PREFIX can be defined with -D from the Makefile */
#ifndef RS_CMD_PREFIX
#define RS_CMD_PREFIX "FT"
#endif

#define RS_INDEX_LIST_CMD RS_CMD_PREFIX "._LIST"
#define RS_CREATE_CMD RS_CMD_PREFIX ".CREATE"
#define RS_ADD_CMD RS_CMD_PREFIX ".ADD"
#define RS_SAFEADD_CMD RS_CMD_PREFIX ".SAFEADD"
#define RS_SETPAYLOAD_CMD RS_CMD_PREFIX ".SETPAYLOAD"
#define RS_ADDHASH_CMD RS_CMD_PREFIX ".ADDHASH"
#define RS_SAFEADDHASH_CMD RS_CMD_PREFIX ".SAFEADDHASH"
#define RS_INFO_CMD RS_CMD_PREFIX ".INFO"
#define RS_SEARCH_CMD RS_CMD_PREFIX ".SEARCH"
#define RS_AGGREGATE_CMD RS_CMD_PREFIX ".AGGREGATE"

#define RS_EXPLAIN_CMD RS_CMD_PREFIX ".EXPLAIN"
#define RS_EXPLAINCLI_CMD RS_CMD_PREFIX ".EXPLAINCLI"
#define RS_DEL_CMD RS_CMD_PREFIX ".DEL"
#define RS_DROP_CMD RS_CMD_PREFIX ".DROP"
#define RS_DROPINDEX_CMD RS_CMD_PREFIX ".DROPINDEX"
#define RS_GET_CMD RS_CMD_PREFIX ".GET"
#define RS_MGET_CMD RS_CMD_PREFIX ".MGET"
#define RS_TAGVALS_CMD RS_CMD_PREFIX ".TAGVALS"

#define RS_SUGADD_CMD RS_CMD_PREFIX ".SUGADD"
#define RS_SUGGET_CMD RS_CMD_PREFIX ".SUGGET"
#define RS_SUGDEL_CMD RS_CMD_PREFIX ".SUGDEL"
#define RS_SUGLEN_CMD RS_CMD_PREFIX ".SUGLEN"

#define RS_CURSOR_CMD RS_CMD_PREFIX ".CURSOR"

#define RS_SYNADD_CMD RS_CMD_PREFIX ".SYNADD"
#define RS_SYNUPDATE_CMD RS_CMD_PREFIX ".SYNUPDATE"
#define RS_SYNFORCEUPDATE_CMD RS_CMD_PREFIX ".SYNFORCEUPDATE"
#define RS_SYNDUMP_CMD RS_CMD_PREFIX ".SYNDUMP"

#define RS_ALTER_CMD RS_CMD_PREFIX ".ALTER"

#define RS_DEBUG RS_CMD_PREFIX ".DEBUG"

#define RS_SPELL_CHECK RS_CMD_PREFIX ".SPELLCHECK"

#define RS_DICT_ADD RS_CMD_PREFIX ".DICTADD"

#define RS_DICT_DEL RS_CMD_PREFIX ".DICTDEL"

#define RS_DICT_DUMP RS_CMD_PREFIX ".DICTDUMP"

#define RS_CONFIG RS_CMD_PREFIX ".CONFIG"

#define RS_ALIASADD RS_CMD_PREFIX ".ALIASADD"
#define RS_ALIASDEL RS_CMD_PREFIX ".ALIASDEL"
#define RS_ALIASUPDATE RS_CMD_PREFIX ".ALIASUPDATE"

#endif

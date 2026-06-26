/*
 * repro_ast_dump.c — TEMPORARY diagnostic (NOT an assertion suite).
 *
 * Prints the tree-sitter S-expression (ts_node_string) for minimal fixtures of
 * the grammars whose def/call extraction is RED, so we can see the exact node
 * kinds + child structure (the vendored grammars ship no node-types.json). This
 * tells us which node kind holds the function name / call so we can extend
 * cbm_resolve_func_name and the lang_specs call/function_node_types precisely.
 *
 * Remove this file once the grammar cluster is fixed.
 */
#include "test_framework.h"
#include "lang_specs.h" /* cbm_ts_language, CBMLanguage, CBM_LANG_* */
#include "tree_sitter/api.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void dump_ast(const char *tag, CBMLanguage lang, const char *src) {
    const TSLanguage *tsl = cbm_ts_language(lang);
    printf("\n===== [AST %s] lang=%d tsl=%s =====\n", tag, (int)lang, tsl ? "ok" : "NULL");
    if (!tsl) {
        return;
    }
    TSParser *p = ts_parser_new();
    if (!ts_parser_set_language(p, tsl)) {
        printf("[AST %s] set_language FAILED (abi?)\n", tag);
        ts_parser_delete(p);
        return;
    }
    TSTree *tree = ts_parser_parse_string(p, NULL, src, (uint32_t)strlen(src));
    if (tree) {
        TSNode root = ts_tree_root_node(tree);
        char *s = ts_node_string(root);
        printf("%s\n", s ? s : "(null sexp)");
        if (s) {
            free(s);
        }
        ts_tree_delete(tree);
    } else {
        printf("[AST %s] parse returned NULL\n", tag);
    }
    ts_parser_delete(p);
}

void suite_repro_ast_dump(void);
void suite_repro_ast_dump(void) {
    dump_ast("scss", CBM_LANG_SCSS,
             "@function add($a) { @return $a; }\n"
             "@mixin flex-center { display: flex; }\n"
             ".x { @include flex-center; }\n");
    dump_ast("nix", CBM_LANG_NIX, "let\n  addOne = x: x + 1;\nin addOne 5\n");
    dump_ast("teal", CBM_LANG_TEAL,
             "local function add(a: number): number\n  return a\nend\nlocal y = add(1)\n");
    dump_ast("move", CBM_LANG_MOVE,
             "module 0x1::M {\n  public fun add(a: u64): u64 { a }\n  public fun run() { add(1); }\n}\n");
    dump_ast("sql", CBM_LANG_SQL,
             "CREATE FUNCTION add(a integer) RETURNS integer AS $$ SELECT a; $$ LANGUAGE sql;\n"
             "SELECT add(1);\n");
    dump_ast("cobol", CBM_LANG_COBOL,
             "       IDENTIFICATION DIVISION.\n"
             "       PROGRAM-ID. RUNPROG.\n"
             "       PROCEDURE DIVISION.\n"
             "           CALL 'HELPER'.\n"
             "           STOP RUN.\n");
    dump_ast("verilog", CBM_LANG_VERILOG,
             "module m;\n"
             "  function integer add(input integer a); add = a; endfunction\n"
             "  initial begin add(1); end\n"
             "endmodule\n");
    dump_ast("elm", CBM_LANG_ELM,
             "module M exposing (..)\nadd a = a\nx = add 1\n");
    dump_ast("agda", CBM_LANG_AGDA, "add : Nat -> Nat\nadd a = add a\n");
    dump_ast("nim", CBM_LANG_NIM, "proc add(a: int): int = a\nlet x = add(1)\n");
}

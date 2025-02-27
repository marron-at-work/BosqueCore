"use strict";

import { runishMainCodeUnsat } from "../../../bin/test/smtoutput/smtemit_nf.js";
import { describe, it } from "node:test";

describe ("SMT evaluate -- String", () => {
    it("should smt eval simple strings", function () {
        runishMainCodeUnsat('public function main(): String { return ""; }', '(assert (not (= "" Main@main)))');
        runishMainCodeUnsat('public function main(): String { return "abc"; }', '(assert (not (= "abc" Main@main)))');
    });
});

describe ("SMT evaluate -- CString", () => {
    it("should smt eval simple cstrings", function () {
        runishMainCodeUnsat("public function main(): CString { return ''; }", '(assert (not (= "" Main@main)))');
        runishMainCodeUnsat("public function main(): CString { return 'abc'; }", '(assert (not (= "abc" Main@main)))');
    });
});

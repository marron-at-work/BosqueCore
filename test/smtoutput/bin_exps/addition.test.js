"use strict";

import { runishMainCodeUnsat, checkProperties } from "../../../bin/test/smtoutput/smtemit_nf.js";
import { describe, it } from "node:test";

describe ("SMT evaluate -- Simple addition", () => {
    it("should smt eval simple", function () {
        runishMainCodeUnsat("public function main(x: Nat): Nat { return x + 2n; }", "(assert (not (= 5 (Main@main 3))))");
    });
});

describe ("SMT check props -- Simple addition", () => {
    it("should smt eval simple", function () {
        checkProperties("public function main(x: Nat): Nat { return x + 2n; }", [{ pkey: ";;--FUNCTION_DECLS--;;", expected: "(define-fun Main@main ((x Nat)) Nat (+ x 2) )" }]);
    });
});



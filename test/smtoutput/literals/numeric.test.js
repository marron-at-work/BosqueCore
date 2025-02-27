"use strict";

import { runishMainCodeUnsat } from "../../../bin/test/smtoutput/smtemit_nf.js";
import { describe, it } from "node:test";

describe ("SMT evaluate -- Nat", () => {
    it("should smt eval simple nats", function () {
        runishMainCodeUnsat("public function main(): Nat { return 0n; }", "(assert (not (= 0 Main@main)))");
        runishMainCodeUnsat("public function main(): Nat { return +2n; }", "(assert (not (= 2 Main@main)))");
    });
});

describe ("SMT evaluate -- Int", () => {
    it("should smt eval simple ints", function () {
        runishMainCodeUnsat("public function main(): Int { return 0i; }", "(assert (not (= 0 Main@main)))");
        runishMainCodeUnsat("public function main(): Int { return -2i; }", "(assert (not (= -2 Main@main)))");
    });
});

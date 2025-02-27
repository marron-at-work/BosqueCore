"use strict";

import { runishMainCodeUnsat } from "../../../bin/test/smtoutput/smtemit_nf.js";
import { describe, it } from "node:test";

describe ("SMT evaluate -- special literals", () => {
    it("should smt eval true", function () {
        runishMainCodeUnsat("public function main(): Bool { return true; }", "(assert (not Main@main))");
    });

    it("should smt eval false", function () {
        runishMainCodeUnsat("public function main(): Bool { return false; }", "(assert Main@main)");
    });
});

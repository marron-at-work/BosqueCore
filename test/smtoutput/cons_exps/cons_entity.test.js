"use strict";

import { runishMainCodeSat, runishMainCodeUnsat } from "../../../bin/test/smtoutput/smtemit_nf.js";
import { describe, it } from "node:test";

describe ("SMT evaluate -- Entity Constructor", () => {
    it("should smt exec positional", function () {
        runishMainCodeUnsat('entity Foo { field f: Int; } public function main(): Int { return Foo{1i}.f; }', "(assert (not (= Main@main 1)))");
        runishMainCodeUnsat('entity Foo { field f: Int; field g: Bool; } public function main(k: Int): Bool { return Foo{k, true}.g; }', "(assert (not (Main@main 3)))");
        runishMainCodeUnsat('entity Foo { field f: Int; field g: Bool; } public function main(k: Int): Int { return Foo{k, true}.f; }', "(assert (not (= (Main@main 3) 3)))");
    });

    it("should smt exec nominal", function () {
        runishMainCodeUnsat('entity Foo { field f: Int; } public function main(): Int { return Foo{f = 1i}.f; }', "(assert (not (= 1 Main@main)))");
        runishMainCodeUnsat('entity Foo { field f: Int; field g: Bool; } public function main(): Bool { return Foo{1i, g = true}.g; }', "(assert (not Main@main))");
        runishMainCodeUnsat('entity Foo { field f: Int; field g: Bool; } public function main(): Bool { return Foo{f=1i, g = true}.g; }', "(assert (not Main@main))");
    });

    it("should smt exec default", function () {
        runishMainCodeUnsat('entity Foo { field f: Int = 0i; } public function main(): Int { return Foo{}.f; }', "(assert (not (= Main@main 0)))");
        runishMainCodeUnsat('entity Foo { field f: Int = 0i; } public function main(): Int { return Foo{5i}.f; }', "(assert (not (= Main@main 5)))");
    });
});

describe ("SMT evaluate -- Entity w/ Invariant Constructor", () => {
    it("should smt exec positional", function () {
        runishMainCodeUnsat('entity Foo { field x: Int; field y: Int; invariant $x >= $y; } public function main(k: Int): Int { let f = Foo{k + 1i, k}; return f.x + f.y; }', "(assert (not (= (Main@main 3) (@Result-ok 7))))");
        runishMainCodeUnsat('entity Foo { field x: Int; field y: Int; invariant $x >= $y; } public function main(k: Int): Int { let f = Foo{k, k + 1i}; return f.x + f.y; }', "(assert (not (is-@Result-err (Main@main 3))))");

        runishMainCodeUnsat('entity Foo { field f: Int; field g: Bool; invariant !$g; invariant $f != 0i; } public function main(): Bool { return Foo{1i, false}.g; }', "(assert (not (= Main@main (@Result-ok false))))");
    });

    it("should smt exec find error", function () {
        runishMainCodeSat('entity Foo { field x: Int; field y: Int; invariant $x >= $y; } public function main(k: Int): Int { let f = Foo{k, 3i}; return f.x + f.y; }', "(declare-const a Int) (assert (not (is-@Result-err (Main@main a))))");
    });

    it("should exec default", function () {
        runishMainCodeUnsat('entity Foo { field f: Int = 0i; invariant $f != 3i; } public function main(): Int { return Foo{5i}.f; }', "(assert (not (= (@Result-ok 5) Main@main)))");
        runishMainCodeUnsat('entity Foo { field f: Int = 0i; invariant $f != 3i; } public function main(): Int { return Foo{}.f; }', "(assert (not (= (@Result-ok 0) Main@main)))");

        runishMainCodeUnsat('entity Foo { field f: Int = 0i; invariant $f != 3i; } public function main(): Int { return Foo{3i}.f; }', "(assert (not (is-@Result-err Main@main)))");
    });
});


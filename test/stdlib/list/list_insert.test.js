"use strict";

import { runMainCode, runMainCodeError } from "../../../bin/test/stdlib/stdlib_nf.js";
import { describe, it } from "node:test";

describe ("List -- insert", () => {
    it("should pushBack", function () {
        runMainCode('public function main(): Nat { return List<Int>{1i, 2i}.pushBack(5i).size(); }', "3n"); 

        runMainCode('public function main(): Int { return List<Int>{1i}.pushBack(2i).back(); }', "2i"); 
        runMainCode('public function main(): Int { return List<Int>{1i, 2i}.pushBack(5i).back(); }', "5i"); 
        runMainCode('public function main(): Int { return List<Int>{1i, 2i}.pushBack(5i).front(); }', "1i"); 
    });

    it("should pushFront", function () {
        runMainCode('public function main(): Nat { return List<Int>{1i, 2i}.pushFront(5i).size(); }', "3n"); 

        runMainCode('public function main(): Int { return List<Int>{1i}.pushFront(2i).front(); }', "2i"); 
        runMainCode('public function main(): Int { return List<Int>{1i, 2i}.pushFront(5i).front(); }', "5i"); 
        runMainCode('public function main(): Int { return List<Int>{1i, 2i}.pushFront(5i).back(); }', "2i"); 
    });

    it("should insert index", function () {
        runMainCode('public function main(): Nat { return List<Int>{1i}.insert(0n, 2i).size(); }', "2n"); 

        runMainCode('public function main(): Int { return List<Int>{1i}.insert(0n, 2i).get(0n); }', "2i"); 
        runMainCode('public function main(): Int { return List<Int>{1i}.insert(0n, 2i).get(1n); }', "1i"); 
        runMainCode('public function main(): Int { return List<Int>{1i}.insert(1n, 2i).get(0n); }', "1i"); 
        runMainCode('public function main(): Int { return List<Int>{1i}.insert(1n, 2i).get(1n); }', "2i"); 

        runMainCode('public function main(): Int { return List<Int>{1i, 2i, 3i}.insert(2n, 5i).get(2n); }', "5i");
        runMainCode('public function main(): Int { return List<Int>{1i, 2i, 3i}.insert(2n, 5i).get(3n); }', "3i");
        runMainCode('public function main(): Int { return List<Int>{1i, 2i, 3i}.insert(3n, 5i).get(2n); }', "3i"); 
        runMainCode('public function main(): Int { return List<Int>{1i, 2i, 3i}.insert(3n, 5i).get(3n); }', "5i"); 
    });

    it("should insert empty", function () {
        runMainCode('public function main(): Nat { return List<Int>{}.pushBack(1i).size(); }', "1n");
        runMainCode('public function main(): Nat { return List<Int>{}.pushFront(1i).size(); }', "1n");
        runMainCode('public function main(): Nat { return List<Int>{}.insert(0n, 1i).size(); }', "1n"); 

        runMainCode('public function main(): Int { return List<Int>{}.pushBack(1i).get(0n); }', "1i");
        runMainCode('public function main(): Int { return List<Int>{}.pushFront(1i).get(0n); }', "1i");
        runMainCode('public function main(): Int { return List<Int>{}.insert(0n, 1i).get(0n); }', "1i"); 
    });

    it("should fail insert out-of-bounds", function () {
        runMainCodeError('public function main(): Int { return List<Int>{}.insert(1n, 5i).get(1n); }', "Error -- i <= this.size() @ list.bsq");
        runMainCodeError('public function main(): Int { return List<Int>{1i, 2i}.insert(3n, 5i).get(1n); }', "Error -- i <= this.size() @ list.bsq");
    });
});

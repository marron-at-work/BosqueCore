// @ts-check
import { executeTypeCheck } from "./typecheck_nf.js";

describe("Generic Constraints", () => {
    it("Should correctly infer U from T in {when T: Option<U>} constraint", () => {
        const bsqcode = `
namespace NS;

entity Foo<T> {
    method {when T: Option<U>} f<U>(v: T): U {
        return v@some;
    }

    method g(v: Option<Int>): Int {
        return this.f<Int>(v); // Explicit U
    }

    method h(v: Option<String>): String {
        return this.f(v); // Implicit U
    }
}

concept C<T> {
    method m(v: T): T;
}

entity Bar<T> {
    method {when T: C<U>} ctest<U>(v: T): U {
        return v.m(NS::Foo<Option<U>>{}.f(none)); //This is a bit contrived to use U deeply
    }
}

// TODO: Add a call to Bar.ctest when a concrete C is available

// Test invocations
const fi = Foo<Option<Int>>{};
const si = Foo<Option<String>>{};
const ni = Foo<Int>{}; // For negative test

export const r1: Int = fi.f(some(10));
export const r2: String = si.f(some("hello"));
// export const r3: Int = ni.f(10); // This should fail type check: T is Int, not Option<U>
// export const r4: String = fi.f(some("bad")); // This should fail: U is Int, arg is Option<String>

function main(): Int {
    let foom = Foo<Option<Int>>{};
    let x: Int = foom.f(some(5));

    let foos = Foo<Option<String>>{};
    let y: String = foos.f(some("five"));

    return x;
}
        `;
        const result = executeTypeCheck(bsqcode);
        expect(result.errors.length).toBe(0); // Expect no errors for valid cases

        // Check inferred types if possible (or add specific assertions later if needed)
        // For now, lack of errors on r1, r2, g, h, main implies success.

        // Test for expected failure: T is Int, not Option<U>
        const bsqcode_fail1 = bsqcode.replace("// export const r3", "export const r3")
                                     .replace("return x;", "return r3;");
        const result_fail1 = executeTypeCheck(bsqcode_fail1);
        expect(result_fail1.errors.length).toBeGreaterThan(0);
        expect(result_fail1.errors.some(err => err.msg.includes("Could not find method f")) || result_fail1.errors.some(err => err.msg.includes("is not a subtype of constraint type"))).toBe(true);


        // Test for expected failure: U is Int (from Foo<Option<Int>>) but arg is Option<String>
        // This specific check is tricky as 'f' itself is generic. The error might manifest as 'String is not Int' or similar.
        const bsqcode_fail2 = bsqcode.replace("// export const r4", "export const r4")
                                     .replace("return x;", "return r4;");
        const result_fail2 = executeTypeCheck(bsqcode_fail2);
        expect(result_fail2.errors.length).toBeGreaterThan(0);
        expect(result_fail2.errors.some(err => err.msg.includes("is not a subtype of"))).toBe(true); // Expecting a subtype error
    });
});

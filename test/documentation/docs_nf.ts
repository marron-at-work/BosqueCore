import * as fs from "fs";
import * as path from "path";

import assert from "node:assert";

import { JSEmitter } from '../../src/backend/jsemitter.js';
import { generateASM, workflowLoadCoreSrc } from '../../src/cmd/workflows.js';
import { Assembly } from '../../src/frontend/assembly.js';
import { InstantiationPropagator } from '../../src/frontend/closed_terms.js';

import { fileURLToPath } from 'url';
import { CodeFileInfo, PackageConfig } from "../../src/frontend/build_decls.js";
import { execSync } from "child_process";
const __dirname = path.dirname(fileURLToPath(import.meta.url));

const bosque_dir: string = path.join(__dirname, "../../../");
const runtime_code_path = path.join(bosque_dir, "bin/jsruntime/runtime.mjs");
const modules_path = path.join(bosque_dir, "node_modules");

import { tmpdir } from 'node:os';
import { TypeChecker } from "../../src/frontend/checker.js";
import { Parser } from "../../src/frontend/parser.js";

function wsnorm(s: string): string {
    return s.trim().replace(/\s+/g, " ");
}

function fromBSONHelper(val: any, type: string): string {
    if(type === "None") {
        return "null";
    }
    else if(type === "Bool") {
        return val ? "true" : "false";
    }
    else if(type === "Nat") {
        return val.toString();
    }
    else if(type === "Int") {
        return val.toString();
    }
    else if(type === "String") {
        return val;
    }
    else if(type === "CString") {
        return val;
    }
    else {
        return `unknown[${val}, ${type}]`;
    }
}

function loadContents(ff: string): Assembly | string {
    const src = workflowLoadCoreSrc();
    if(src === undefined) {
        return "**ERROR**";
    }

    const maintest: CodeFileInfo[]  = [{srcpath: "test", filename: "test", contents: ff}];

    const rr = Parser.parse(src, maintest, ["EXEC_LIBS"]);
    return Array.isArray(rr) ? rr[0].message : rr;
}

function buildAssembly(srcfile: string): Assembly | undefined {
    const userpackage = new PackageConfig(["EXEC_LIBS"], [{ srcpath: "test.bsq", filename: "test.bsq", contents: srcfile }]);
    const [asm, perrors, terrors] = generateASM(userpackage);

    if(perrors.length === 0 && terrors.length === 0) {
        return asm;
    }
    else {
        return undefined;
    }
}

function buildMainCode(assembly: Assembly, outname: string) {
    const iim = InstantiationPropagator.computeInstantiations(assembly, "Main");
    const [jscode, _] = JSEmitter.emitAssembly(assembly, "debug", "test", iim);

    const nndir = path.normalize(outname);
    try {
        fs.cpSync(runtime_code_path, path.join(nndir, "runtime.mjs"));
        fs.cpSync(modules_path, path.join(nndir, "node_modules"), { recursive: true });

        for(let i = 0; i < jscode.length; ++i) {
            const fname = path.join(nndir, `${jscode[i].ns.ns[0]}.mjs`);
            fs.writeFileSync(fname, jscode[i].contents);
        }
    }
    catch(e) {  
        return false;
    }

    return true;
}

function checkTestFunctionInFile(code: string) {
    const assembly = loadContents("declare namespace Main;\n\n" + code);

    if(typeof(assembly) === "string") {
        assert.fail(assembly);
    }

    const errors = TypeChecker.checkAssembly(assembly);
    if(errors.length > 0) {
        assert.fail(errors.map(e => e.msg).join("\n"));
    }
}

function checkTestFunctionInFileError(code: string, msg: string) {
    const assembly = loadContents("declare namespace Main;\n\n" + code);

    if(typeof(assembly) === "string") {
        assert.equal(assembly, msg);
    }
    else {
        const errors = TypeChecker.checkAssembly(assembly);
        assert.equal(errors[0].msg, msg);
    }
}

function execMainCode(code: string): string {
    const nndir = fs.mkdtempSync(path.join(tmpdir(), "bosque-test-"));

    let result = "";
    try {
        const asm = buildAssembly("declare namespace Main;\n\n" + code);
        if(asm === undefined) {
            result = "[FAILED TO BUILD ASSEMBLY]";
        }
        else {
            if(!buildMainCode(asm, nndir)) {
                result = "[FAILED TO BUILD MAIN CODE]";
            }
            else {
                try {
                    const mjs = path.join(nndir, "Main.mjs");
                    result = execSync(`node ${mjs}`).toString();
                }
                catch(e) {
                    result = "[FAILED TO RUN MAIN CODE]";
                }
            }
        }
    }
    catch(e) {
        result = `[Unexpected error ${e}]`;
    }
    finally {
        fs.rmSync(nndir, { recursive: true });
    }

    return wsnorm(result);
}

function runMainCode(code: string, expected: [any, string]) {
    const result = execMainCode(code);

    assert.equal(wsnorm(result), fromBSONHelper(expected[0], expected[1]));
}


function runMainCodeError(code: string, expected: string) {
    const result = execMainCode(code);

    assert.equal(wsnorm(result).replace(/:\d+$/, ""), expected);
}

export {
    checkTestFunctionInFile, checkTestFunctionInFileError,
    runMainCode, runMainCodeError
};
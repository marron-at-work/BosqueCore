import assert from "node:assert";

import { TypeSignature } from "./type.js";
import { BinderInfo } from "./body.js";

class VarInfo {
    readonly srcname: string;
    readonly vtype: TypeSignature;

    readonly isConst: boolean;
    readonly mustDefined: boolean;

    constructor(srcname: string, vtype: TypeSignature, isConst: boolean, mustDefined: boolean) {
        this.srcname = srcname;
        this.vtype = vtype;

        this.isConst = isConst;
        this.mustDefined = mustDefined;
    }

    updateTypeAndDefine(ttype: TypeSignature): VarInfo {
        return new VarInfo(this.srcname, ttype, this.isConst, true);
    }

    updateDefine(): VarInfo {
        return new VarInfo(this.srcname, this.vtype, this.isConst, true);
    }
}

abstract class TypeInferContext {
    static asSimpleType(ctx: TypeInferContext | undefined): TypeSignature | undefined {
        if(ctx === undefined) {
            return undefined
        }
        else {
            return ctx instanceof SimpleTypeInferContext ? ctx.ttype : undefined;
        }
    }

    static asEListOptions(ctx: TypeInferContext | undefined): (TypeSignature | undefined)[] | undefined {
        if(ctx === undefined) {
            return undefined
        }
        else {
            return ctx instanceof EListStyleTypeInferContext ? ctx.elist : undefined;
        }
    }
}

class SimpleTypeInferContext extends TypeInferContext {
    readonly ttype: TypeSignature;

    constructor(ttype: TypeSignature) {
        super();
        this.ttype = ttype;
    }
}

class EListStyleTypeInferContext extends TypeInferContext {
    readonly elist: (TypeSignature | undefined)[];

    constructor(elist: (TypeSignature | undefined)[]) {
        super();
        this.elist = elist;
    }
}

class TypeEnvironment {
    readonly normalflow: boolean;
    readonly returnflow: boolean;
    readonly parent: TypeEnvironment | undefined;

    readonly args: VarInfo[];
    readonly declReturnType: TypeSignature;
    readonly inferReturn: TypeInferContext;

    readonly locals: VarInfo[][];

    constructor(normalflow: boolean, returnflow: boolean, parent: TypeEnvironment | undefined, args: VarInfo[], returnType: TypeSignature, inferctx: TypeInferContext, locals: VarInfo[][]) {
        this.normalflow = normalflow;
        this.returnflow = returnflow;
        this.parent = parent;

        this.args = args;
        this.declReturnType = returnType;
        this.inferReturn = inferctx;

        this.locals = locals;
    }

    private static cloneLocals(locals: VarInfo[][]): VarInfo[][] {
        return locals.map((l) => [...l]);
    }

    static createInitialStdEnv(args: VarInfo[], declReturnType: TypeSignature, inferReturn: TypeInferContext): TypeEnvironment {
        return new TypeEnvironment(true, false, undefined, args, declReturnType, inferReturn, []);
    }

    static createInitialLambdaEnv(args: VarInfo[], declReturnType: TypeSignature, inferReturn: TypeInferContext, enclosing: TypeEnvironment): TypeEnvironment {
        return new TypeEnvironment(true, false, enclosing, args, declReturnType, inferReturn, []);
    }

    resolveLambdaCaptureVarType(vname: string): TypeSignature | undefined {
        let parent = this.parent;
        while(parent !== undefined) {
            const vv = parent.resolveLocalVarInfo(vname);
            if(vv !== undefined) {
                return vv.vtype;
            }
        }

        return undefined;
    }

    resolveLocalVarInfo(vname: string): VarInfo | undefined {
        for(let i = this.locals.length - 1; i >= 0; i--) {
            for(let j = 0; j < this.locals[i].length; j++) {
                if(this.locals[i][j].srcname === vname) {
                    return this.locals[i][j];
                }
            }
        }

        return this.args.find((v) => v.srcname === vname);
    }

    addLocalVariable(vname: string, vtype: TypeSignature, isConst: boolean, mustDefined: boolean): TypeEnvironment {
        return new TypeEnvironment(this.normalflow, this.returnflow, this.parent, [...this.args], this.declReturnType, this.inferReturn, [...TypeEnvironment.cloneLocals(this.locals), [new VarInfo(vname, vtype, isConst, mustDefined)]]);
    }

    assignLocalVariable(vname: string): TypeEnvironment {
        let locals: VarInfo[][] = [];
        for(let i = this.locals.length - 1; i >= 0; i--) {
            let frame: VarInfo[] = [];
            for(let j = 0; j < this.locals[i].length; j++) {
                if(this.locals[i][j].srcname !== vname) {
                    frame.push(this.locals[i][j]);
                }
                else {
                    frame.push(this.locals[i][j].updateDefine());
                }
            }

            locals = [frame, ...locals];
        }

        return new TypeEnvironment(this.normalflow, this.returnflow, this.parent, [...this.args], this.declReturnType, this.inferReturn, locals);
    }

    retypeLocalVariable(vname: string, ttype: TypeSignature): TypeEnvironment {
        let locals: VarInfo[][] = [];
        for(let i = this.locals.length - 1; i >= 0; i--) {
            let frame: VarInfo[] = [];
            for(let j = 0; j < this.locals[i].length; j++) {
                if(this.locals[i][j].srcname !== vname) {
                    frame.push(this.locals[i][j]);
                }
                else {
                    frame.push(this.locals[i][j].updateTypeAndDefine(ttype));
                }
            }

            locals = [frame, ...locals];
        }

        return new TypeEnvironment(this.normalflow, this.returnflow, this.parent, [...this.args], this.declReturnType, this.inferReturn, locals);
    }

    setDeadFlow(): TypeEnvironment {
        return new TypeEnvironment(false, false, this.parent, [...this.args], this.declReturnType, this.inferReturn, TypeEnvironment.cloneLocals(this.locals));
    }

    setReturnFlow(): TypeEnvironment {
        return new TypeEnvironment(false, true, this.parent, [...this.args], this.declReturnType, this.inferReturn, TypeEnvironment.cloneLocals(this.locals));
    }

    pushNewLocalScope(): TypeEnvironment {
        return new TypeEnvironment(this.normalflow, this.returnflow, this, [...this.args], this.declReturnType, this.inferReturn, [...TypeEnvironment.cloneLocals(this.locals), []]);
    }

    pushNewLocalBinderScope(vname: string, vtype: TypeSignature): TypeEnvironment {
        return new TypeEnvironment(this.normalflow, this.returnflow, this, [...this.args], this.declReturnType, this.inferReturn, [...TypeEnvironment.cloneLocals(this.locals), [new VarInfo(vname, vtype, true, true)]]);
    }

    popLocalScope(): TypeEnvironment {
        assert(this.locals.length > 0);
        return new TypeEnvironment(this.normalflow, this.returnflow, this.parent, [...this.args], this.declReturnType, this.inferReturn, TypeEnvironment.cloneLocals(this.locals).slice(0, this.locals.length - 1));
    }

    static mergeEnvironmentsSimple(origenv: TypeEnvironment, ...envs: TypeEnvironment[]): TypeEnvironment {
        let locals: VarInfo[][] = [];
        for(let i = 0; i < origenv.locals.length; i++) {
            let frame: VarInfo[] = [];

            for(let j = 0; j < origenv.locals[i].length; j++) {
                const mdef = envs.every((e) => (e.resolveLocalVarInfo(origenv.locals[i][j].srcname) as VarInfo).mustDefined);
                frame.push(new VarInfo(origenv.locals[i][j].srcname, origenv.locals[i][j].vtype, origenv.locals[i][j].isConst, mdef));
            }

            locals.push(frame);
        }

        return new TypeEnvironment(origenv.normalflow, origenv.returnflow, origenv.parent, [...origenv.args], origenv.declReturnType, origenv.inferReturn, locals);
    }

    static gatherEnvironmentsOptBinderType(binfo: BinderInfo | undefined, ...envs: TypeEnvironment[]): TypeSignature[] | undefined {
        if(binfo === undefined) {
            return undefined;
        }
        else {
            const topts = envs.filter((e) => e.normalflow).map((e) => e.resolveLocalVarInfo(binfo.scopename) as VarInfo);
            return topts.length !== 0 ? topts.map((v) => v.vtype) : undefined;
        }
    }

    static mergeEnvironmentsOptBinderFlow(origenv: TypeEnvironment, binfo: BinderInfo | undefined, refinetype: TypeSignature | undefined, ...envs: TypeEnvironment[]): TypeEnvironment {
        const menv = TypeEnvironment.mergeEnvironmentsSimple(origenv, ...envs);

        if(binfo === undefined || refinetype === undefined) {
            return menv;
        }
        else {
            const refinevar = binfo.scopename;
            return menv.retypeLocalVariable(refinevar.slice(1), refinetype)
        }
    }
}

export {
    VarInfo,
    TypeInferContext, SimpleTypeInferContext, EListStyleTypeInferContext,
    TypeEnvironment
};

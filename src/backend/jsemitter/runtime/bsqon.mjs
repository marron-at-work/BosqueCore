"use strict;"

import { validateStringLiteral, validateCStringLiteral } from "@bosque/jsbrex";

////////////////////////////////////////////////////////////////////////////////
//Support

function NOT_IMPLEMENTED(name) {
    throw new ParseError(new SourceInfo(0, 0, 0, 0) `Not implemented: ${name}`);
}

function SourceInfo(line, column, pos, span) {
    this.line = line;
    this.column = column;
    this.pos = pos;
    this.span = span;
}

function Token(line, column, cpos, span, kind, data) {
    this.line = line;
    this.column = column;
    this.pos = cpos;
    this.span = span;
    this.kind = kind;
    this.data = data;
}
Token.prototype.getSourceInfo = function() {
    return new SourceInfo(this.line, this.column, this.pos, this.span);
}

function ParserError(sinfo, message) {
    this.sinfo = sinfo;
    this.message = message;
}

const TokenStrings = {
    Clear: "[CLEAR]",
    Error: "[ERROR]",

    Nat: "[LITERAL_NAT]",
    Int: "[LITERAL_INT]",
    BigNat: "[LITERAL_BIGNAT]",
    BigInt: "[LITERAL_BIGINT]",
    Float: "[LITERAL_FLOAT]",
    Decimal: "[LITERAL_DECIMAL]",
    Rational: "[LITERAL_RATIONAL]",
    DecimalDegree: "[LITERAL_DECIMAL_DEGREE]",
    LatLong: "[LITERAL_LATLONG]",
    Complex: "[LITERAL_COMPLEX]",
    
    ByteBuffer: "[LITERAL_BYTEBUFFER]",
    UUIDValue: "[LITERAL_UUID]",
    ShaHashcode: "[LITERAL_SHA]",

    String: "[LITERAL_STRING]",
    CString: "[LITERAL_EX_STRING]",

    Regex: "[LITERAL_REGEX]",
    PathItem: "[LITERAL_PATH_ITEM]",
    PathTemplateItem: "[LITERAL_TEMPLATE_PATH_ITEM]",

    TZDateTime: "[LITERAL_TZTIME]",
    TAITime: "[LITERAL_TAI_TIME]",
    PlainDate: "[LITERAL_PLAIN_DATE]",
    PlainTime: "[LITERAL_PLAIN_TIME]",
    LogicalTime: "[LITERAL_LOGICAL_TIME]",
    Timestamp: "[LITERAL_TIMESTAMP]",

    DeltaDateTime: "[LITERAL_DELTA_DATETIME]",
    DeltaSeconds: "[LITERAL_DELTA_SECONDS]",
    DeltaLogicalTime: "[LITERAL_DELTA_LOGICAL_TIME]",
    DeltaTimestamp: "[LITERAL_DELTA_TIMESTAMP]",

    IdentifierName: "[IDENTIFIER]",
    
    EndOfStream: "[EOS]"
};

////////////////////////////////////////////////////////////////////////////////
//Keywords

const KW_SRC = "$src";
const KW_none = "none";
const KW_true = "true";
const KW_false = "false";
const KW_SOME = "some";
const KW_OK = "ok";
const KW_FAIL = "fail";
const KW_LET = "let";
const KW_IN = "in";

const KeywordStrings = [
    KW_SRC,
    KW_none,
    KW_true,
    KW_false,
    KW_SOME,
    KW_OK,
    KW_FAIL,
    KW_LET,
    KW_IN
].sort((a, b) => { return (a.length !== b.length) ? (b.length - a.length) : ((a !== b) ? (a < b ? -1 : 1) : 0); });

////////////////////////////////////////////////////////////////////////////////
//Symbols

const SYM_lbrack = "[";
const SYM_lparen = "(";
const SYM_lbrace = "{";
const SYM_lbrackbar = "[|";
const SYM_lparenbar = "(|";

const SYM_rbrack = "]";
const SYM_rparen = ")";
const SYM_rbrace = "}";
const SYM_rbrackbar = "|]";
const SYM_rparenbar = "|)";
const SYM_langle = "<";
const SYM_rangle = ">";

const SYM_coloncolon = "::";
const SYM_hash = "#";
const SYM_bigarrow = " => ";
const SYM_colon = ":";
const SYM_coma = ",";
const SYM_dot = ".";
const SYM_eq = "=";

const StandardSymbols = [
    SYM_colon,
    SYM_coloncolon,
    SYM_coma,
    SYM_dot,
    SYM_eq,
    SYM_HOLE,
    
    SYM_hash,
].sort((a, b) => { return (a.length !== b.length) ? (b.length - a.length) : ((a !== b) ? (a < b ? -1 : 1) : 0); });

const SpaceRequiredSymbols = [
    SYM_bigarrow,
].map((s) => { return s.trim(); })
.sort((a, b) => { return (a.length !== b.length) ? (b.length - a.length) : ((a !== b) ? (a < b ? -1 : 1) : 0); });

const ParenSymbols = [
    SYM_lbrack,
    SYM_lparen,
    SYM_lbrace,
    SYM_lbrackbar,
    SYM_lparenbar,
    SYM_langle,

    SYM_rbrack,
    SYM_rparen,
    SYM_rbrace,
    SYM_rparenbar,
    SYM_rbrackbar,
    SYM_rangle

].sort((a, b) => { return (a.length !== b.length) ? (b.length - a.length) : ((a !== b) ? (a < b ? -1 : 1) : 0); });

////////////////////////////////////////////////////////////////////////////////
//BSQON Lexer and parser

const _s_whitespaceRe = new RegExp('\\s+', "y");

const _s_nonzeroIntValNoSign = '[1-9][0-9]*';
const _s_nonzeroIntVal = `[+-]?${_s_nonzeroIntValNoSign}`;
const _s_intValueNoSign = `(0|${_s_nonzeroIntValNoSign})`;
const _s_intValue = `(0|[+]0|${_s_nonzeroIntVal})`;

const _s_floatValueNoSign = '[0-9]+[.][0-9]+([eE][-+]?[0-9]+)?';
const _s_floatValue = `[+-]?${_s_floatValueNoSign}([eE][-+]?[0-9]+)?`;

const _s_floatSimpleValueNoSign = '[0-9]+[.][0-9]+';
const _s_floatSimpleValue = `[+-]?${_s_floatSimpleValueNoSign}`;

const _s_intNumberinoRe = new RegExp(`${_s_intValue}`, "y");

const _s_intRe = new RegExp(`(${_s_intValue})i`, "y");
const _s_natRe = new RegExp(`(${_s_intValue})n`, "y");
const _s_bigintRe = new RegExp(`(${_s_intValue})I`, "y");
const _s_bignatRe = new RegExp(`(${_s_intValue})N`, "y");

const _s_floatRe = new RegExp(`(${_s_floatValue})f`, "y");

/*
    private static readonly _s_decimalRe = new RegExp(`(${Lexer._s_floatValue})d`, "y");
    private static readonly _s_rationalRe = new RegExp(`(${Lexer._s_intValue})(/(${Lexer._s_nonzeroIntValNoSign}))?R`, "y");
    private static readonly _s_complexRe = new RegExp(`(${Lexer._s_floatValue})[+-](${Lexer._s_floatValueNoSign})j`, "y");

    private static readonly _s_decimalDegreeRe = new RegExp(`(${Lexer._s_floatSimpleValue})dd`, "y");
    private static readonly _s_latlongRe = new RegExp(`(${Lexer._s_floatSimpleValue})lat[+-]${Lexer._s_floatSimpleValueNoSign}long`, "y");
    
    private static readonly _s_logicaltimeRe = new RegExp(`(${Lexer._s_intValue})l`, "y");

    private static readonly _s_deltasecondsRE = new RegExp(`[+-](${Lexer._s_floatValueNoSign})ds`, "y");
    private static readonly _s_deltalogicaltimeRE = new RegExp(`[+-](${Lexer._s_intValueNoSign})dl`, "y");

    private static readonly _s_zerodenomRationalNumberinoRe = new RegExp(`(${Lexer._s_intValue})/0`, "y");
    private static readonly _s_zerodenomRationalRe = new RegExp(`(${Lexer._s_intValue})/0R`, "y");

    private static _s_bytebufferRe = new RegExp('0x\\[[0-9a-fA-F]+\\]', "y");
    private static _s_uuidRe = new RegExp('uuid[47]\\{[a-fA-F0-9]{8}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{12}\\}', "y");
    private static _s_shaRe = new RegExp('sha3\\{[a-fA-F0-9]{64}\\}', "y");
*/

const _s_validCStringChars = /^[ -~\t\n]*$/;
const _s_validRegexChars = /([!-.0-~]|\s)+/;

/*
    private static _s_datevalue = '([0-9]{4})-([0-9]{2})-([0-9]{2})';
    private static _s_timevalue = '([0-9]{2}):([0-9]{2}):([0-9]{2})';
    private static _s_tzvalue = '((\{[a-zA-Z0-9/, _-]+\})|[A-Z]+)';

    private static _s_datatimeRE = new RegExp(`${Lexer._s_datevalue}T${Lexer._s_timevalue}@${Lexer._s_tzvalue}`, "y");
    private static _s_taitimeRE = new RegExp(`${Lexer._s_datevalue}T${Lexer._s_timevalue}?`, "y");
    private static _s_plaindateRE = new RegExp(`${Lexer._s_datevalue}`, "y");
    private static _s_plaintimeRE = new RegExp(`${Lexer._s_timevalue}`, "y");
    private static _s_timestampRE = new RegExp(`${Lexer._s_datevalue}T${Lexer._s_timevalue}[.]([0-9]{3})Z`, "y");
*/


const _s_identiferName = new RegExp('[$]?[_a-zA-Z][_a-zA-Z0-9]*', "y");

const _s_redundantSignRE = /[+-]{2,}/y;

const MIN_SAFE_INT = -4611686018427387903n;
const MAX_SAFE_INT = 4611686018427387903n;

//negation and conversion are always safe
const MAX_SAFE_NAT = 4611686018427387903n;

function BSQONLexer(input) {
    this.input = input;
    this.jsStrPos = 0;
    
    this.cline = 0;
    this.linestart = 0;
    
    this.tokens = [];
}
BSQONLexer.prototype.advancePosition = function(epos) {
    this.jsStrPos += epos - this.jsStrPos;
}
/**
 * @param {RegExp} re
 * @returns {string | null}
 */
BSQONLexer.prototype.trylex = function(re) {
    re.lastIndex = this.jsStrPos;
    const mm = re.exec(this.input);
    if(mm === null) {
        return null;
    }
    else {
        return mm[0];
    }
}
/**
 * 
 * @param {string[]} options 
 * @param {RegExp} suffix 
 * @returns {string | null}
 */
BSQONLexer.prototype.trylexWSPlus = function(options, suffix) {
    Lexer._s_whitespaceRe.lastIndex = this.jsStrPos;
    const mmpre = Lexer._s_whitespaceRe.exec(this.input);
    if(mmpre === null) {
        return null;
    }
    
    const mopt = options.find((opt) => this.input.startsWith(opt, this.jsStrPos + mmpre[0].length));
    if(mopt === undefined) {
        return null;
    }

    suffix.lastIndex = this.jsStrPos + mmpre[0].length + mopt.length;
    const mmsuf = suffix.exec(this.input);
    if(mmsuf === null) {
        return null;
    }

    return this.input.slice(this.jsStrPos, this.jsStrPos + mmpre[0].length + mopt.length + mmsuf[0].length);
}
/**
 * @param {SourceInfo} sinfo 
 * @param {string} message 
 * @throws {ParserError}
 */
BSQONLexer.prototype.raiseError = function(sinfo, message) {
    throw new ParserError(sinfo, message);
}
/**
 * @param {number} jsepos
 * @param {string} kind
 * @returns {Token}
 */
BSQONLexer.prototype.recordLexToken = function(jsepos, kind) {
    this.tokens.push(new Token(this.cline, this.jsStrPos - this.linestart, this.jsStrPos, jsepos - this.jsStrPos, kind, kind)); //set data to kind string
    this.advancePosition(jsepos);
}
/**
 * @param {number} jsepos
 * @param {string} kind
 * @param {string} data
 * @returns {Token}
 */
BSQONLexer.prototype.recordLexTokenWData = function(jsepos, kind, data) {
    this.tokens.push(new Token(this.cline, this.jsStrPos - this.linestart, this.jsStrPos, jsepos - this.jsStrPos, kind, data));
    this.advancePosition(jsepos);
}
/**
 * @param {number} jspos
 * @param {number} jepos
 * @returns {void}
 */
BSQONLexer.prototype.updatePositionInfo = function(jspos, jepos) {
    for (let i = jspos; i < jepos; ++i) {
        if (this.input[i] === "\n") {
            this.cline++;
            this.linestart = i + 1;
        }
    }
}
/**
 * @returns {boolean}
 */
BSQONLexer.prototype.tryLexWS = function() {
    const arop = this.trylexWSPlus(SpaceRequiredSymbols, Lexer._s_whitespaceRe);
    if (arop !== null) {
        return false;
    }

    const m = this.trylex(_s_whitespaceRe);
    if (m === null) {
        return false;
    }

    this.updatePositionInfo(this.jsStrPos, this.jsStrPos + m.length);
    this.advancePosition(this.jsStrPos + m.length);

    return true;
}
/**
 * @returns {boolean}
 */
BSQONLexer.prototype.tryLexLineComment = function() {
    const m = this.input.startsWith("%%", this.jsStrPos);
    if (!m) {
        return false;
    }

    let jepos = this.input.indexOf("\n", this.jsStrPos);

    if (jepos === -1) {
        this.updatePositionInfo(this.jsStrPos, this.jsStrEnd);
        this.advancePosition(this.jsStrEnd);
    }
    else {
        jepos++;

        this.updatePositionInfo(this.jsStrPos, jepos);
        this.advancePosition(jepos);
    }

    return true;
}
/**
 * @returns {void}
 * @throws {ParserError}
 */
BSQONLexer.prototype.checkRedundantSigns = function() {
    if(this.jsStrPos !== this.jsStrEnd && (this.input[this.jsStrPos] === "+" || this.input[this.jsStrPos] === "-")) {
        _s_redundantSignRE.lastIndex = this.jsStrPos;
        const mm = _s_redundantSignRE.exec(this.input);
        if(mm !== null) {
            this.raiseError(new SourceInfo(this.cline, this.linestart, this.jsStrPos, mm[0].length), "Redundant sign in number");
        }
    }
}
/**
 * @returns {boolean}
 */
BSQONLexer.prototype.tryLexFloatCompositeLikeToken = function() {
    return false;
}
/**
 * @returns {boolean}
 */
BSQONLexer.prototype.tryLexFloatLikeToken = function() {
    const mfloat = this.trylex(_s_floatRe);
    if(mfloat !== null) {
        this.recordLexTokenWData(this.jsStrPos + mfloat.length, TokenStrings.Float, mfloat);
        return true;
    }

    return false;
}
/**
 * @returns {boolean}
 */
BSQONLexer.prototype.tryLexIntegralCompositeLikeToken = function() {
    return false;
}
/**
 * @returns {boolean}
 */
BSQONLexer.prototype.tryLexIntegralLikeToken = function() {
    const mint = this.trylex(_s_intRe);
    if(mint !== null) {
        this.recordLexTokenWData(this.jsStrPos + mint.length, TokenStrings.Int, mint);
        return true;
    }

    const mnat = this.trylex(_s_natRe);
    if(mnat !== null) {
        this.recordLexTokenWData(this.jsStrPos + mnat.length, TokenStrings.Nat, mnat);
        return true;
    }

    const mbigint = this.trylex(_s_bigintRe);
    if(mbigint !== null) {
        this.recordLexTokenWData(this.jsStrPos + mbigint.length, TokenStrings.BigInt, mbigint);
        return true;
    }

    const mbignat = this.trylex(_s_bignatRe);
    if(mbignat !== null) {
        this.recordLexTokenWData(this.jsStrPos + mbignat.length, TokenStrings.BigNat, mbignat);
        return true;
    }

    const mnumberino = this.trylex(_s_intNumberinoRe);
    if(mnumberino !== null) {
        this.recordLexTokenWData(this.jsStrPos + mnumberino.length, TokenStrings.NumberinoInt, mnumberino);
        return true;
    }

    return false;
}
/**
 * @returns {boolean}
 * @throws {ParserError}
 */
BSQONLexer.prototype.tryLexNumberLikeToken = function() {
    this.checkRedundantSigns();

    const cft = this.tryLexFloatCompositeLikeToken();
    if(cft) {
        return true;
    }
    
    const ft = this.tryLexFloatLikeToken();
    if(ft) {
        return true;
    }

    const cit = this.tryLexIntegralCompositeLikeToken();
    if(cit) {
        return true;
    }

    const it = this.tryLexIntegralLikeToken();
    if(it) {
        return true;
    }

    return false;
}
/**
 * @returns {boolean}
 */
BSQONLexer.prototype.tryLexByteBuffer = function() {
    return false;
}
/**
 * @returns {boolean}
 */
BSQONLexer.prototype.tryLexUUID = function() {
    return false;
}
/**
 * @returns {boolean}
 */
BSQONLexer.prototype.tryLexHashCode = function() {
    return false;
}
/**
 * @returns {boolean}
 * @throws {ParserError}
 */
BSQONLexer.prototype.tryLexUnicodeString = function() {
    let ncpos = this.jsStrPos + 1;
    
    let jepos = this.input.indexOf('"', ncpos);
    if(jepos === -1) {
        this.raiseError(new SourceInfo(this.cline, this.linestart, this.jsStrPos, this.jsStrEnd - this.jsStrPos), "Unterminated string literal");
    }

    jepos++;
    let strval = this.input.slice(this.jsStrPos, jepos);

    this.updatePositionInfo(this.jsStrPos, jepos);
    this.recordLexTokenWData(jepos, TokenStrings.String, strval);
    return true;
}
/**
 * @returns {boolean}
 * @throws {ParserError}
 */
BSQONLexer.prototype.tryLexCString = function() {
    let ncpos = this.jsStrPos + 1;
        
    let jepos = this.input.indexOf("'", ncpos);
    if(jepos === -1) {
        this.raiseError(new SourceInfo(this.cline, this.linestart, this.jsStrPos, this.jsStrEnd - this.jsStrPos), "Unterminated CString literal");
    }
    
    const mstr = this.input.slice(ncpos, jepos);
    if(!_s_validCStringChars.test(mstr)) {
        this.raiseError(new SourceInfo(this.cline, this.linestart, this.jsStrPos, jepos - this.jsStrPos), "Invalid chacaters in CString literal");
    }

    jepos++;
    let strval = this.input.slice(this.jsStrPos, jepos);

    this.updatePositionInfo(this.jsStrPos, jepos);
    this.recordLexTokenWData(jepos, istemplate ? TokenStrings.TemplateCString : TokenStrings.CString, strval);
    return true;
}
/**
 * @returns {boolean}
 * @throws {ParserError}
 */
BSQONLexer.prototype.tryLexStringLike = function() {
    const us = this.tryLexUnicodeString();
    if(us) {
        return true;
    }

    const as = this.tryLexCString();
    if(as) {
        return true;
    }

    return false;
}
/**
 * @returns {boolean}
 * @throws {ParserError}
 */
BSQONLexer.prototype.tryLexRegex = function() {
    if(!this.input.startsWith("/", this.jsStrPos)) {
        return false;
    }

    let jepos = this.input.indexOf("/", this.jsStrPos + 1);
    if(jepos === -1) {
            this.raiseError(new SourceInfo(this.cline, this.linestart, this.jsStrPos, this.jsStrEnd - this.jsStrPos), "Unterminated Regex literal");
    }
    
    const mstr = this.input.slice(this.jsStrPos + 1, jepos);
    if(!_s_validRegexChars.test(mstr)) {
        this.raiseError(new SourceInfo(this.cline, this.linestart, this.jsStrPos, jepos - this.jsStrPos), "Invalid characters in (or empty) Regex literal");
    }

    jepos++;
    if(this.input.startsWith("c", jepos) || this.input.startsWith("p", jepos)) {
        jepos++;
    }

    let strval = this.input.slice(this.jsStrPos, jepos);

    this.updatePositionInfo(this.jsStrPos, jepos);
    this.recordLexTokenWData(jepos, TokenStrings.Regex, strval);
    return true;
}
/**
 * @returns {boolean}
 * @throws {ParserError}
 */
BSQONLexer.prototype.tryLexPath = function() {
    return false;
}
/**
 * @returns {boolean}
 * @throws {ParserError}
 */
BSQONLexer.prototype.tryLexDateTime = function() {
    return false;
}
/**
 * @returns {boolean}
 * @throws {ParserError}
 */
BSQONLexer.prototype.tryLexDateTimeDelta = function() {
    return false;
}
/**
 * @returns {boolean}
 * @throws {ParserError}
 */
BSQONLexer.prototype.tryLexDateLike = function() {
    const mdd = this.tryLexDateTimeDelta();
    if(mdd) {
        return true;
    }

    const mdt = this.tryLexDateTime();
    if(mdt) {
        return true;
    }

    return false;
}
/**
 * @returns {boolean}
 * @throws {ParserError}
 */
BSQONLexer.prototype.tryLexSymbol = function() {
    const spaceop = this.trylexWSPlus(SpaceRequiredSymbols, _s_whitespaceRe);
    if(spaceop !== null) {
        const realstr = " " + spaceop.trim() + " ";

        this.recordLexToken(this.jsStrPos + spaceop.length, realstr);
        return true; 
    }
    else {
        const mm = ParenSymbols.find((value) => this.input.startsWith(value, this.jsStrPos)) || StandardSymbols.find((value) => this.input.startsWith(value, this.jsStrPos));
        if(mm !== undefined) {
            this.recordLexToken(this.jsStrPos + mm.length, mm);
            return true;
        }

        
        return false;
    }
}
/**
 * @returns {boolean}
 */
BSQONLexer.prototype.tryLexName = function() {
    const identifiermatch = this.trylex(_s_identiferName);
    const kwmatch = KeywordStrings.find((value) => this.input.startsWith(value, this.jsStrPos));

    if(identifiermatch === null && kwmatch === undefined) {
        return false;
    }

    if (identifiermatch !== null && kwmatch === undefined) {
        this.recordLexTokenWData(this.jsStrPos + identifiermatch.length, TokenStrings.IdentifierName, identifiermatch);
    }
    else if(identifiermatch === null && kwmatch !== undefined) {
        this.recordLexToken(this.jsStrPos + kwmatch.length, kwmatch);
    }
    else {
        const nnid = identifiermatch;
        const nnkw = kwmatch;

        if (nnid.length > nnkw.length) {
            this.recordLexTokenWData(this.jsStrPos + nnid.length, TokenStrings.IdentifierName, nnid);
        }
        else {
            this.recordLexToken(this.jsStrPos + nnkw.length, nnkw);
        }
    }

    return true;
}
/**
 * @returns {boolean}
 * @throws {ParserError}
 */
BSQONLexer.prototype.lex = function() {
    while (this.jsStrPos < this.jsStrEnd) {
        if (this.tryLexWS() || this.tryLexLineComment() || this.tryLexDocComment() || this.tryLexSpanComment()) {
            //continue
        }
        else if(this.tryLexDateLike()) {
            //continue
        }
        else if(this.tryLexStringLike()) {
            //continue
        }
        else if(this.tryLexByteBuffer() || this.tryLexUUID() || this.tryLexHashCode()) {
            //continue
        }
        else if (this.tryLexNumberLikeToken()) {
            //continue
        }
        else if(this.tryLexAttribute()) {
            //continue
        }
        else if (this.tryLexSymbol() || this.tryLexName()) {
            //continue
        }
        else if (this.tryLexPath() || this.tryLexRegex()) {
            //continue
        }
        else {
            this.raiseError(new ParserError(this.srcfile, new SourceInfo(this.cline, this.linestart, this.jsStrPos, 1), "Unrecognized token"));
        }
    }
    this.recordLexToken(this.input.length, TokenStrings.EndOfStream);
    
    return this.tokens;
}

/**
 * @param {string} nv 
 * @param {BigInt | null} upper
 * @returns {BigInt}
 * @throws {ParserError}
 */
function isValidNat(nv, upper) {
    try {
        const vv = BigInt(nv);
        if(vv < 0) {
            throw new ParserError(new SourceInfo(0, 0, 0, 0), "Nat value outside of valid range");
        }
        if(upper !== null && vv > upper) {
            throw new ParserError(new SourceInfo(0, 0, 0, 0), "Nat value outside of valid range");
        }

        return vv;
    }
    catch(e) {
        throw new ParserError(new SourceInfo(0, 0, 0, 0), "Invalid Nat value");
    }
}

/**
 * @param {string} nv 
 * @param {BigInt | null} lower
 * @param {BigInt | null} upper
 * @returns {BigInt}
 * @throws {ParserError}
 */
function isValidInt(nv) {
    try {
        const vv = BigInt(nv);
        if(lower !== null && vv < lower) {
            throw new ParserError(new SourceInfo(0, 0, 0, 0), "Int value outside of valid range");
        }
        if(upper !== null && vv > upper) {
            throw new ParserError(new SourceInfo(0, 0, 0, 0), "Int value outside of valid range");
        }
    }
    catch(e) {
        throw new ParserError(new SourceInfo(0, 0, 0, 0), "Invalid Int");
    }
}

/**
 * @param {string} nv 
 * @returns {number}
 * @throws {ParserError}
 */
function isValidFloat(nv) {
    try {
        return parseFloat(nv);
    }
    catch(e) {
        throw new ParserError(new SourceInfo(0, 0, 0, 0), "Invalid Float");
    }
}

function BSQONParser(tokens, noneval, pfmap) {
    this.tokens = tokens;
    this.idx = 0;

    this.noneval = noneval;
    this.pfmap = pfmap;
}
/**
 * @returns {Token}
 */
BSQONParser.prototype.peek = function() {
    if(this.idx >= this.tokens.length) {
        return new Token(0, 0, 0, 0, TokenStrings.EndOfStream, TokenStrings.EndOfStream);
    }
    else {
        return this.tokens[this.idx];
    }
}
/**
 * @returns {Token}
 */
BSQONParser.prototype.consume = function() {
    if(this.idx >= this.tokens.length) {
        return new Token(0, 0, 0, 0, TokenStrings.EndOfStream, TokenStrings.EndOfStream);
    }
    else {
        return this.tokens[this.idx++];
    }
}
/**
 * @returns {string}
 * @throws {ParserError}
 */
BSQONParser.prototype.consumeAndGetData = function() {
    if(this.idx >= this.tokens.length) {
        throw new ParserError(new SourceInfo(0, 0, 0, 0), "Unexpected end of stream");
    }
    else {
        return this.tokens[this.idx++].data;
    }
}
/**
 * @param {string} kind
 * @throws {ParserError}
 */
BSQONParser.prototype.consumeExpected = function(kind) {
    const tok = this.consume();
    if(tok.kind !== kind) {
        throw new ParserError(tok.sinfo, `Expected token ${kind} but found ${tok.kind}`);
    }
}
/**
 * @param {string} kind
 * @throws {ParserError}
 */
BSQONParser.prototype.consumeExpectedAndGetData = function(kind) {
    const tok = this.consume();
    if(tok.kind !== kind) {
        throw new ParserError(tok.sinfo, `Expected token ${kind} but found ${tok.kind}`);
    }

    return tok.data;
}
/**
 * @returns {boolean}
 */
BSQONParser.prototype.test = function(kind) {
    return this.peek().kind === kind;
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseNone = function() {
    this.consumeExpected(KW_none);
    return this.noneval;
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseBool = function() {
    if(!(this.test(KW_true) || this.test(KW_false))) {
        throw new ParserError(this.peek().sinfo, "Expected boolean literal");
    }
    else {
        this.consumeAndGetData() === KW_true ? true : false;
    }
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseNat = function() {
    const nv = this.consumeExpectedAndGetData(TokenStrings.Nat);
    return isValidNat(nv.slice(0, -1), MAX_SAFE_NAT);
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseInt = function() {
    const nv = this.consumeExpectedAndGetData(TokenStrings.Int);
    return isValidNat(nv.slice(0, -1), MIN_SAFE_INT, MAX_SAFE_INT);
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseBigNat = function() {
    const nv = this.consumeExpectedAndGetData(TokenStrings.BigNat);
    return isValidNat(nv.slice(0, -1), null);
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseBigInt = function() {
    const nv = this.consumeExpectedAndGetData(TokenStrings.BigInt);
    return isValidNat(nv.slice(0, -1), null, null);
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseRational = function() {
    NOT_IMPLEMENTED("parseRational");
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseFloat = function() {
    const nv = this.consumeExpectedAndGetData(TokenStrings.Float);
    return isValidFloat(nv.slice(0, -1));
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseDecimal = function() {
    NOT_IMPLEMENTED("parseDecimal");
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseDecimalDegree = function() {
    NOT_IMPLEMENTED("parseDecimalDegree");
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseLatLong = function() {
    NOT_IMPLEMENTED("parseLatLong");
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseComplex = function() {
    NOT_IMPLEMENTED("parseComplex");
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseString = function() {
    const ss = this.consumeExpectedAndGetData(TokenStrings.String);
    try {
        return validateUnicodeStringLiteral(ss);
    }
    catch(e) {
        throw new ParserError(this.peek().sinfo, "Invalid Unicode string literal");
    }
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseCString = function() {
    const ss = this.consumeExpectedAndGetData(TokenStrings.CString);
    try {
        return validateCStringLiteral(ss);
    }
    catch(e) {
        throw new ParserError(this.peek().sinfo, "Invalid CString literal");
    }
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseByteBuffer = function() {
    NOT_IMPLEMENTED("parseByteBuffer");
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseUUIDv4 = function() {
    NOT_IMPLEMENTED("parseUUIDv4");
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseUUIDv7 = function() {
    NOT_IMPLEMENTED("parseUUIDv7");
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseSHAHashcode = function() {
    NOT_IMPLEMENTED("parseSHAHashcode");
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseTZDateTime = function() {
    NOT_IMPLEMENTED("parseTZDateTime");
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseTIATime = function() {
    NOT_IMPLEMENTED("parseTIATime");
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parsePlainDate = function() {
    NOT_IMPLEMENTED("parsePlainDate");
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parsePlainTime = function() {
    NOT_IMPLEMENTED("parsePlainTime");
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseLogicalTime = function() {
    NOT_IMPLEMENTED("parseLogicalTime");
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseISOTimeStamp = function() {
    NOT_IMPLEMENTED("parseISOTimeStamp");
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseDeltaDateTime = function() {
    NOT_IMPLEMENTED("parseDeltaDateTime");
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseDeltaSeconds = function() {
    NOT_IMPLEMENTED("parseDeltaSeconds");
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseDeltaLogical = function() {
    NOT_IMPLEMENTED("parseDeltaLogical");
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseDeltaISOTimeStamp = function() {
    NOT_IMPLEMENTED("parseDeltaISOTimeStamp");
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseUnicodeRegex = function() {
    //TODO: We need to do better here...
    return this.consumeExpectedAndGetData(TokenStrings.Regex);
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseCRegex = function() {
    //TODO: We need to do better here...
    return this.consumeExpectedAndGetData(TokenStrings.CRegex);
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parsePathRegex = function() {
    NOT_IMPLEMENTED("parsePathRegex");
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parsePath = function() {
    NOT_IMPLEMENTED("parsePath");
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parsePathItem = function() {
    NOT_IMPLEMENTED("parsePathItem");
}
/**
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parsePathGlob = function() {
    NOT_IMPLEMENTED("parsePathGlob");
}
/**
 * @param {string} tkey
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseValuePrimitive = function(tkey) {
    if(tkey === "None") {
        return this.parseNone();
    }
    else if(tkey === "Bool") {
        return this.parseBool();
    }
    else if(tkey === "Int") {
        return this.parseInt();
    }
    else if(tkey === "Nat") {
        return this.parseNat();
    }
    else if(tkey === "BigInt") {
        return this.parseBigInt();
    }
    else if(tkey === "BigNat") {
        return this.parseBigNat();
    }
    else if(tkey === "Rational") {
        return this.parseRational();
    }
    else if(tkey === "Float") {
        return this.parseFloat();
    }
    else if(tkey === "Decimal") {
        return this.parseDecimal();
    }
    else if(tkey === "DecimalDegree") {
        return this.parseDecimalDegree();
    }
    else if(tkey === "LatLongCoordinate") {
        return this.parseLatLong();
    }
    else if(tkey === "Complex") {
        return this.parseComplex();
    }
    else if(tkey === "String") {
        return this.parseString();
    }
    else if(tkey === "CString") {
        return this.parseCString();
    }
    else if(tkey === "ByteBuffer") {
        return this.parseByteBuffer();
    }
    else if(tkey === "TZDateTime") {
        return this.parseTZDateTime();
    }
    else if(tkey === "TIATime") {
        return this.parseTIATime();
    }
    else if(tkey === "PlainDate") {
        return this.parsePlainDate();
    }
    else if(tkey === "PlainTime") {
        return this.parsePlainTime();
    }
    else if(tkey === "LogicalTime") {
        return this.parseLogicalTime();
    }
    else if(tkey === "ISOTimeStamp") {
        return thiparseISOTimeStamp();
    }
    else if(tkey === "UUIDv4") {
        return this.parseUUIDv4();
    }
    else if(tkey === "UUIDv7") {
        return this.parseUUIDv7();
    }
    else if(tkey === "SHAContentHash") {
        return this.parseSHAHashcode();
    }
    else if(tkey === "DataTimeDelta") {
        return this.parseDeltaDateTime();
    }
    else if(tkey === "SecondsDelta") {
        return this.parseDeltaSeconds();
    }
    else if(tkey === "ISOTimestampDelta") {
        return this.parseDeltaISOTimeStamp();
    }
    else if(tkey === "LogicalTimeDelta") {
        return this.parseDeltaLogical();
    }      
    else if(tkey === "UnicodeRegex") {
        return this.parseUnicodeRegex();
    }
    else if(tkey === "CRegex") {
        return this.parseCRegex();
    }
    else if(tkey === "PathRegex") {
        return this.parsePathRegex();
    }
    else if(tkey === "Path") {
        return this.parsePath();
    }
    else if(tkey === "PathItem") {
        return this.parsePathItem();
    }
    else if(tkey === "Glob") {
        return this.parseGlob();
    }
    else {
        throw new ParserError(this.peek().sinfo, `Unknown primitive type: ${tkey}`);
    }
}
/**
 * @param {string} typekey 
 * @returns {any}
 * @throws {ParserError}
 */
BSQONParser.prototype.parseValue = function(typekey) {
    if(this.test(TokenStrings.EndOfStream)) {
        xxxx;
    }
    //handle primitive types
    //dispatch on typekey
    xxxx;
}


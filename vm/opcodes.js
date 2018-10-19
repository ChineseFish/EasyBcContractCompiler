// name, baseCost, off stack, on stack, dynamic, async
const codes = {
  0x00: ['ADD', 2, 1],
  0x01: ['SUB', 2, 1],
  0x02: ['MUL', 2, 1],
  0x03: ['DIV', 2, 1],
  0x04: ['MOD', 2, 1],
  0X05: ['NEG', 1, 1],

  0x10: ['LT', 2, 1],
  0x11: ['GT', 2, 1],
  0x12: ['SLT', 2, 1],
  0x13: ['SGT', 2, 1],
  0x14: ['EQ', 2, 1],
  0x15: ['NEQ', 2, 1],
  0x16: ['AND', 2, 1],
  0x17: ['OR', 2, 1],
  0x18: ['NOT', 1, 1],

  0x20: ['BAND', 2, 1],
  0x21: ['BOR', 2, 1],
  0x22: ['XOR', 2, 1],
  0x23: ['SHL', 2, 1],
  0x24: ['SHR', 2, 1],

  0x30: ['POP', 1, 0],

  0x40: ['JUMP', 1, 0],
  0x41: ['JUMPI', 2, 0],

  0x60: ['PUSH', 0, 1],
  0x61: ['PUSH', 0, 1],
  0x62: ['PUSH', 0, 1],
  0x63: ['PUSH', 0, 1],
  0x64: ['PUSH', 0, 1],
  0x65: ['PUSH', 0, 1],
  0x66: ['PUSH', 0, 1],
  0x67: ['PUSH', 0, 1],
  0x68: ['PUSH', 0, 1],
  0x69: ['PUSH', 0, 1],
  0x6a: ['PUSH', 0, 1],
  0x6b: ['PUSH', 0, 1],
  0x6c: ['PUSH', 0, 1],
  0x6d: ['PUSH', 0, 1],
  0x6e: ['PUSH', 0, 1],
  0x6f: ['PUSH', 0, 1],
  0x70: ['PUSH', 0, 1],
  0x71: ['PUSH', 0, 1],
  0x72: ['PUSH', 0, 1],
  0x73: ['PUSH', 0, 1],
  0x74: ['PUSH', 0, 1],
  0x75: ['PUSH', 0, 1],
  0x76: ['PUSH', 0, 1],
  0x77: ['PUSH', 0, 1],
  0x78: ['PUSH', 0, 1],
  0x79: ['PUSH', 0, 1],
  0x7a: ['PUSH', 0, 1],
  0x7b: ['PUSH', 0, 1],
  0x7c: ['PUSH', 0, 1],
  0x7d: ['PUSH', 0, 1],
  0x7e: ['PUSH', 0, 1],
  0x7f: ['PUSH', 0, 1],

  0x80: ['DUP', 0, 1],
  0x81: ['DUP', 0, 1],
  0x82: ['DUP', 0, 1],
  0x83: ['DUP', 0, 1],
  0x84: ['DUP', 0, 1],
  0x85: ['DUP', 0, 1],
  0x86: ['DUP', 0, 1],
  0x87: ['DUP', 0, 1],
  0x88: ['DUP', 0, 1],
  0x89: ['DUP', 0, 1],
  0x8a: ['DUP', 0, 1],
  0x8b: ['DUP', 0, 1],
  0x8c: ['DUP', 0, 1],
  0x8d: ['DUP', 0, 1],
  0x8e: ['DUP', 0, 1],
  0x8f: ['DUP', 0, 1],

  0x90: ['SWAP', 0, 0],
  0x91: ['SWAP', 0, 0],
  0x92: ['SWAP', 0, 0],
  0x93: ['SWAP', 0, 0],
  0x94: ['SWAP', 0, 0],
  0x95: ['SWAP', 0, 0],
  0x96: ['SWAP', 0, 0],
  0x97: ['SWAP', 0, 0],
  0x98: ['SWAP', 0, 0],
  0x99: ['SWAP', 0, 0],
  0x9a: ['SWAP', 0, 0],
  0x9b: ['SWAP', 0, 0],
  0x9c: ['SWAP', 0, 0],
  0x9d: ['SWAP', 0, 0],
  0x9e: ['SWAP', 0, 0],
  0x9f: ['SWAP', 0, 0]
}

module.exports = function (op, full) {
  var code = codes[op] ? codes[op] : ['INVALID', 0, 0]
  var opcode = code[0]

  if (full) {

    if (opcode === 'PUSH') {
      opcode += op - 0x5f
    }

    if (opcode === 'DUP') {
      opcode += op - 0x7f
    }

    if (opcode === 'SWAP') {
      opcode += op - 0x8f
    }
  }

  /* in表示指令需要的参数个数，out表示指令返回的内容的个数 */
  return {name: opcode, opcode: op, in: code[1], out: code[2]}
}
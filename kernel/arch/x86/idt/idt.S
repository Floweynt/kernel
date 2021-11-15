.text

.global idt_handler_entries
.global idt_entry_start
.global idt_entry_end

idt_common:
    cld
    addq $8, %rsp
	pushq %rax
    movq -0x10(%rsp), %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    // setup stack frame
    pushq %rbp
    movq %rsp, %rbp

    // parameters
    movq %rax, %rsi
    movq %rbp, %rdi
	call *idt_handler_entries(%rax)

    popq %rbp
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdi
    popq %rsi
    popq %rdx
    popq %rcx
    popq %rbx
    popq %rax
    subq $0x18, %rsp
    iretq

idt_entry_start:


    // auto generated IDT handler stubs


idt_0:
    push 0
    push $0
    jmp idt_common
idt_entry_end:

idt_1:
    push 0
    push $1
    jmp idt_common

idt_2:
    push 0
    push $2
    jmp idt_common

idt_3:
    push 0
    push $3
    jmp idt_common

idt_4:
    push 0
    push $4
    jmp idt_common

idt_5:
    push 0
    push $5
    jmp idt_common

idt_6:
    push 0
    push $6
    jmp idt_common

idt_7:
    push 0
    push $7
    jmp idt_common

idt_8:
    push $8
    jmp idt_common
    ud2

idt_9:
    push 0
    push $9
    jmp idt_common

idt_10:
    push $10
    jmp idt_common
    ud2

idt_11:
    push $11
    jmp idt_common
    ud2

idt_12:
    push $12
    jmp idt_common
    ud2

idt_13:
    push $13
    jmp idt_common
    ud2

idt_14:
    push $14
    jmp idt_common
    ud2

idt_15:
    push 0
    push $15
    jmp idt_common

idt_16:
    push 0
    push $16
    jmp idt_common

idt_17:
    push $17
    jmp idt_common
    ud2

idt_18:
    push 0
    push $18
    jmp idt_common

idt_19:
    push 0
    push $19
    jmp idt_common

idt_20:
    push 0
    push $20
    jmp idt_common

idt_21:
    push 0
    push $21
    jmp idt_common

idt_22:
    push 0
    push $22
    jmp idt_common

idt_23:
    push 0
    push $23
    jmp idt_common

idt_24:
    push 0
    push $24
    jmp idt_common

idt_25:
    push 0
    push $25
    jmp idt_common

idt_26:
    push 0
    push $26
    jmp idt_common

idt_27:
    push 0
    push $27
    jmp idt_common

idt_28:
    push 0
    push $28
    jmp idt_common

idt_29:
    push 0
    push $29
    jmp idt_common

idt_30:
    push 0
    push $30
    jmp idt_common

idt_31:
    push 0
    push $31
    jmp idt_common

idt_32:
    push 0
    push $32
    jmp idt_common

idt_33:
    push 0
    push $33
    jmp idt_common

idt_34:
    push 0
    push $34
    jmp idt_common

idt_35:
    push 0
    push $35
    jmp idt_common

idt_36:
    push 0
    push $36
    jmp idt_common

idt_37:
    push 0
    push $37
    jmp idt_common

idt_38:
    push 0
    push $38
    jmp idt_common

idt_39:
    push 0
    push $39
    jmp idt_common

idt_40:
    push 0
    push $40
    jmp idt_common

idt_41:
    push 0
    push $41
    jmp idt_common

idt_42:
    push 0
    push $42
    jmp idt_common

idt_43:
    push 0
    push $43
    jmp idt_common

idt_44:
    push 0
    push $44
    jmp idt_common

idt_45:
    push 0
    push $45
    jmp idt_common

idt_46:
    push 0
    push $46
    jmp idt_common

idt_47:
    push 0
    push $47
    jmp idt_common

idt_48:
    push 0
    push $48
    jmp idt_common

idt_49:
    push 0
    push $49
    jmp idt_common

idt_50:
    push 0
    push $50
    jmp idt_common

idt_51:
    push 0
    push $51
    jmp idt_common

idt_52:
    push 0
    push $52
    jmp idt_common

idt_53:
    push 0
    push $53
    jmp idt_common

idt_54:
    push 0
    push $54
    jmp idt_common

idt_55:
    push 0
    push $55
    jmp idt_common

idt_56:
    push 0
    push $56
    jmp idt_common

idt_57:
    push 0
    push $57
    jmp idt_common

idt_58:
    push 0
    push $58
    jmp idt_common

idt_59:
    push 0
    push $59
    jmp idt_common

idt_60:
    push 0
    push $60
    jmp idt_common

idt_61:
    push 0
    push $61
    jmp idt_common

idt_62:
    push 0
    push $62
    jmp idt_common

idt_63:
    push 0
    push $63
    jmp idt_common

idt_64:
    push 0
    push $64
    jmp idt_common

idt_65:
    push 0
    push $65
    jmp idt_common

idt_66:
    push 0
    push $66
    jmp idt_common

idt_67:
    push 0
    push $67
    jmp idt_common

idt_68:
    push 0
    push $68
    jmp idt_common

idt_69:
    push 0
    push $69
    jmp idt_common

idt_70:
    push 0
    push $70
    jmp idt_common

idt_71:
    push 0
    push $71
    jmp idt_common

idt_72:
    push 0
    push $72
    jmp idt_common

idt_73:
    push 0
    push $73
    jmp idt_common

idt_74:
    push 0
    push $74
    jmp idt_common

idt_75:
    push 0
    push $75
    jmp idt_common

idt_76:
    push 0
    push $76
    jmp idt_common

idt_77:
    push 0
    push $77
    jmp idt_common

idt_78:
    push 0
    push $78
    jmp idt_common

idt_79:
    push 0
    push $79
    jmp idt_common

idt_80:
    push 0
    push $80
    jmp idt_common

idt_81:
    push 0
    push $81
    jmp idt_common

idt_82:
    push 0
    push $82
    jmp idt_common

idt_83:
    push 0
    push $83
    jmp idt_common

idt_84:
    push 0
    push $84
    jmp idt_common

idt_85:
    push 0
    push $85
    jmp idt_common

idt_86:
    push 0
    push $86
    jmp idt_common

idt_87:
    push 0
    push $87
    jmp idt_common

idt_88:
    push 0
    push $88
    jmp idt_common

idt_89:
    push 0
    push $89
    jmp idt_common

idt_90:
    push 0
    push $90
    jmp idt_common

idt_91:
    push 0
    push $91
    jmp idt_common

idt_92:
    push 0
    push $92
    jmp idt_common

idt_93:
    push 0
    push $93
    jmp idt_common

idt_94:
    push 0
    push $94
    jmp idt_common

idt_95:
    push 0
    push $95
    jmp idt_common

idt_96:
    push 0
    push $96
    jmp idt_common

idt_97:
    push 0
    push $97
    jmp idt_common

idt_98:
    push 0
    push $98
    jmp idt_common

idt_99:
    push 0
    push $99
    jmp idt_common

idt_100:
    push 0
    push $100
    jmp idt_common

idt_101:
    push 0
    push $101
    jmp idt_common

idt_102:
    push 0
    push $102
    jmp idt_common

idt_103:
    push 0
    push $103
    jmp idt_common

idt_104:
    push 0
    push $104
    jmp idt_common

idt_105:
    push 0
    push $105
    jmp idt_common

idt_106:
    push 0
    push $106
    jmp idt_common

idt_107:
    push 0
    push $107
    jmp idt_common

idt_108:
    push 0
    push $108
    jmp idt_common

idt_109:
    push 0
    push $109
    jmp idt_common

idt_110:
    push 0
    push $110
    jmp idt_common

idt_111:
    push 0
    push $111
    jmp idt_common

idt_112:
    push 0
    push $112
    jmp idt_common

idt_113:
    push 0
    push $113
    jmp idt_common

idt_114:
    push 0
    push $114
    jmp idt_common

idt_115:
    push 0
    push $115
    jmp idt_common

idt_116:
    push 0
    push $116
    jmp idt_common

idt_117:
    push 0
    push $117
    jmp idt_common

idt_118:
    push 0
    push $118
    jmp idt_common

idt_119:
    push 0
    push $119
    jmp idt_common

idt_120:
    push 0
    push $120
    jmp idt_common

idt_121:
    push 0
    push $121
    jmp idt_common

idt_122:
    push 0
    push $122
    jmp idt_common

idt_123:
    push 0
    push $123
    jmp idt_common

idt_124:
    push 0
    push $124
    jmp idt_common

idt_125:
    push 0
    push $125
    jmp idt_common

idt_126:
    push 0
    push $126
    jmp idt_common

idt_127:
    push 0
    push $127
    jmp idt_common

idt_128:
    push 0
    push $128
    jmp idt_common

idt_129:
    push 0
    push $129
    jmp idt_common

idt_130:
    push 0
    push $130
    jmp idt_common

idt_131:
    push 0
    push $131
    jmp idt_common

idt_132:
    push 0
    push $132
    jmp idt_common

idt_133:
    push 0
    push $133
    jmp idt_common

idt_134:
    push 0
    push $134
    jmp idt_common

idt_135:
    push 0
    push $135
    jmp idt_common

idt_136:
    push 0
    push $136
    jmp idt_common

idt_137:
    push 0
    push $137
    jmp idt_common

idt_138:
    push 0
    push $138
    jmp idt_common

idt_139:
    push 0
    push $139
    jmp idt_common

idt_140:
    push 0
    push $140
    jmp idt_common

idt_141:
    push 0
    push $141
    jmp idt_common

idt_142:
    push 0
    push $142
    jmp idt_common

idt_143:
    push 0
    push $143
    jmp idt_common

idt_144:
    push 0
    push $144
    jmp idt_common

idt_145:
    push 0
    push $145
    jmp idt_common

idt_146:
    push 0
    push $146
    jmp idt_common

idt_147:
    push 0
    push $147
    jmp idt_common

idt_148:
    push 0
    push $148
    jmp idt_common

idt_149:
    push 0
    push $149
    jmp idt_common

idt_150:
    push 0
    push $150
    jmp idt_common

idt_151:
    push 0
    push $151
    jmp idt_common

idt_152:
    push 0
    push $152
    jmp idt_common

idt_153:
    push 0
    push $153
    jmp idt_common

idt_154:
    push 0
    push $154
    jmp idt_common

idt_155:
    push 0
    push $155
    jmp idt_common

idt_156:
    push 0
    push $156
    jmp idt_common

idt_157:
    push 0
    push $157
    jmp idt_common

idt_158:
    push 0
    push $158
    jmp idt_common

idt_159:
    push 0
    push $159
    jmp idt_common

idt_160:
    push 0
    push $160
    jmp idt_common

idt_161:
    push 0
    push $161
    jmp idt_common

idt_162:
    push 0
    push $162
    jmp idt_common

idt_163:
    push 0
    push $163
    jmp idt_common

idt_164:
    push 0
    push $164
    jmp idt_common

idt_165:
    push 0
    push $165
    jmp idt_common

idt_166:
    push 0
    push $166
    jmp idt_common

idt_167:
    push 0
    push $167
    jmp idt_common

idt_168:
    push 0
    push $168
    jmp idt_common

idt_169:
    push 0
    push $169
    jmp idt_common

idt_170:
    push 0
    push $170
    jmp idt_common

idt_171:
    push 0
    push $171
    jmp idt_common

idt_172:
    push 0
    push $172
    jmp idt_common

idt_173:
    push 0
    push $173
    jmp idt_common

idt_174:
    push 0
    push $174
    jmp idt_common

idt_175:
    push 0
    push $175
    jmp idt_common

idt_176:
    push 0
    push $176
    jmp idt_common

idt_177:
    push 0
    push $177
    jmp idt_common

idt_178:
    push 0
    push $178
    jmp idt_common

idt_179:
    push 0
    push $179
    jmp idt_common

idt_180:
    push 0
    push $180
    jmp idt_common

idt_181:
    push 0
    push $181
    jmp idt_common

idt_182:
    push 0
    push $182
    jmp idt_common

idt_183:
    push 0
    push $183
    jmp idt_common

idt_184:
    push 0
    push $184
    jmp idt_common

idt_185:
    push 0
    push $185
    jmp idt_common

idt_186:
    push 0
    push $186
    jmp idt_common

idt_187:
    push 0
    push $187
    jmp idt_common

idt_188:
    push 0
    push $188
    jmp idt_common

idt_189:
    push 0
    push $189
    jmp idt_common

idt_190:
    push 0
    push $190
    jmp idt_common

idt_191:
    push 0
    push $191
    jmp idt_common

idt_192:
    push 0
    push $192
    jmp idt_common

idt_193:
    push 0
    push $193
    jmp idt_common

idt_194:
    push 0
    push $194
    jmp idt_common

idt_195:
    push 0
    push $195
    jmp idt_common

idt_196:
    push 0
    push $196
    jmp idt_common

idt_197:
    push 0
    push $197
    jmp idt_common

idt_198:
    push 0
    push $198
    jmp idt_common

idt_199:
    push 0
    push $199
    jmp idt_common

idt_200:
    push 0
    push $200
    jmp idt_common

idt_201:
    push 0
    push $201
    jmp idt_common

idt_202:
    push 0
    push $202
    jmp idt_common

idt_203:
    push 0
    push $203
    jmp idt_common

idt_204:
    push 0
    push $204
    jmp idt_common

idt_205:
    push 0
    push $205
    jmp idt_common

idt_206:
    push 0
    push $206
    jmp idt_common

idt_207:
    push 0
    push $207
    jmp idt_common

idt_208:
    push 0
    push $208
    jmp idt_common

idt_209:
    push 0
    push $209
    jmp idt_common

idt_210:
    push 0
    push $210
    jmp idt_common

idt_211:
    push 0
    push $211
    jmp idt_common

idt_212:
    push 0
    push $212
    jmp idt_common

idt_213:
    push 0
    push $213
    jmp idt_common

idt_214:
    push 0
    push $214
    jmp idt_common

idt_215:
    push 0
    push $215
    jmp idt_common

idt_216:
    push 0
    push $216
    jmp idt_common

idt_217:
    push 0
    push $217
    jmp idt_common

idt_218:
    push 0
    push $218
    jmp idt_common

idt_219:
    push 0
    push $219
    jmp idt_common

idt_220:
    push 0
    push $220
    jmp idt_common

idt_221:
    push 0
    push $221
    jmp idt_common

idt_222:
    push 0
    push $222
    jmp idt_common

idt_223:
    push 0
    push $223
    jmp idt_common

idt_224:
    push 0
    push $224
    jmp idt_common

idt_225:
    push 0
    push $225
    jmp idt_common

idt_226:
    push 0
    push $226
    jmp idt_common

idt_227:
    push 0
    push $227
    jmp idt_common

idt_228:
    push 0
    push $228
    jmp idt_common

idt_229:
    push 0
    push $229
    jmp idt_common

idt_230:
    push 0
    push $230
    jmp idt_common

idt_231:
    push 0
    push $231
    jmp idt_common

idt_232:
    push 0
    push $232
    jmp idt_common

idt_233:
    push 0
    push $233
    jmp idt_common

idt_234:
    push 0
    push $234
    jmp idt_common

idt_235:
    push 0
    push $235
    jmp idt_common

idt_236:
    push 0
    push $236
    jmp idt_common

idt_237:
    push 0
    push $237
    jmp idt_common

idt_238:
    push 0
    push $238
    jmp idt_common

idt_239:
    push 0
    push $239
    jmp idt_common

idt_240:
    push 0
    push $240
    jmp idt_common

idt_241:
    push 0
    push $241
    jmp idt_common

idt_242:
    push 0
    push $242
    jmp idt_common

idt_243:
    push 0
    push $243
    jmp idt_common

idt_244:
    push 0
    push $244
    jmp idt_common

idt_245:
    push 0
    push $245
    jmp idt_common

idt_246:
    push 0
    push $246
    jmp idt_common

idt_247:
    push 0
    push $247
    jmp idt_common

idt_248:
    push 0
    push $248
    jmp idt_common

idt_249:
    push 0
    push $249
    jmp idt_common

idt_250:
    push 0
    push $250
    jmp idt_common

idt_251:
    push 0
    push $251
    jmp idt_common

idt_252:
    push 0
    push $252
    jmp idt_common

idt_253:
    push 0
    push $253
    jmp idt_common

idt_254:
    push 0
    push $254
    jmp idt_common

idt_255:
    push 0
    push $255
    jmp idt_common

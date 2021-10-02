.text

.global idt_handler_entries
.global idt_entry_start
.global idt_entry_end

idt_common:
    cld
	pushq %rax
    movq 8(%rsp), %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsp
    pushq %rbp
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
    pushq %rbp
    movq %rsp, %rbp
    movq %rax, %rsi
    movq %rbp, %rdi
	call *idt_handler_entries(%rax)

idt_entry_start:
idt_0:
    and $~0xf, %rsp
    pushq $0
    jmp idt_common
idt_entry_end:
idt_1:
    and $~0xf, %rsp
    pushq $1
    jmp idt_common

idt_2:
    and $~0xf, %rsp
    pushq $2
    jmp idt_common

idt_3:
    and $~0xf, %rsp
    pushq $3
    jmp idt_common

idt_4:
    and $~0xf, %rsp
    pushq $4
    jmp idt_common

idt_5:
    and $~0xf, %rsp
    pushq $5
    jmp idt_common

idt_6:
    and $~0xf, %rsp
    pushq $6
    jmp idt_common

idt_7:
    and $~0xf, %rsp
    pushq $7
    jmp idt_common

idt_8:
    and $~0xf, %rsp
    pushq $8
    jmp idt_common

idt_9:
    and $~0xf, %rsp
    pushq $9
    jmp idt_common

idt_10:
    and $~0xf, %rsp
    pushq $10
    jmp idt_common

idt_11:
    and $~0xf, %rsp
    pushq $11
    jmp idt_common

idt_12:
    and $~0xf, %rsp
    pushq $12
    jmp idt_common

idt_13:
    and $~0xf, %rsp
    pushq $13
    jmp idt_common

idt_14:
    and $~0xf, %rsp
    pushq $14
    jmp idt_common

idt_15:
    and $~0xf, %rsp
    pushq $15
    jmp idt_common

idt_16:
    and $~0xf, %rsp
    pushq $16
    jmp idt_common

idt_17:
    and $~0xf, %rsp
    pushq $17
    jmp idt_common

idt_18:
    and $~0xf, %rsp
    pushq $18
    jmp idt_common

idt_19:
    and $~0xf, %rsp
    pushq $19
    jmp idt_common

idt_20:
    and $~0xf, %rsp
    pushq $20
    jmp idt_common

idt_21:
    and $~0xf, %rsp
    pushq $21
    jmp idt_common

idt_22:
    and $~0xf, %rsp
    pushq $22
    jmp idt_common

idt_23:
    and $~0xf, %rsp
    pushq $23
    jmp idt_common

idt_24:
    and $~0xf, %rsp
    pushq $24
    jmp idt_common

idt_25:
    and $~0xf, %rsp
    pushq $25
    jmp idt_common

idt_26:
    and $~0xf, %rsp
    pushq $26
    jmp idt_common

idt_27:
    and $~0xf, %rsp
    pushq $27
    jmp idt_common

idt_28:
    and $~0xf, %rsp
    pushq $28
    jmp idt_common

idt_29:
    and $~0xf, %rsp
    pushq $29
    jmp idt_common

idt_30:
    and $~0xf, %rsp
    pushq $30
    jmp idt_common

idt_31:
    and $~0xf, %rsp
    pushq $31
    jmp idt_common

idt_32:
    and $~0xf, %rsp
    pushq $32
    jmp idt_common

idt_33:
    and $~0xf, %rsp
    pushq $33
    jmp idt_common

idt_34:
    and $~0xf, %rsp
    pushq $34
    jmp idt_common

idt_35:
    and $~0xf, %rsp
    pushq $35
    jmp idt_common

idt_36:
    and $~0xf, %rsp
    pushq $36
    jmp idt_common

idt_37:
    and $~0xf, %rsp
    pushq $37
    jmp idt_common

idt_38:
    and $~0xf, %rsp
    pushq $38
    jmp idt_common

idt_39:
    and $~0xf, %rsp
    pushq $39
    jmp idt_common

idt_40:
    and $~0xf, %rsp
    pushq $40
    jmp idt_common

idt_41:
    and $~0xf, %rsp
    pushq $41
    jmp idt_common

idt_42:
    and $~0xf, %rsp
    pushq $42
    jmp idt_common

idt_43:
    and $~0xf, %rsp
    pushq $43
    jmp idt_common

idt_44:
    and $~0xf, %rsp
    pushq $44
    jmp idt_common

idt_45:
    and $~0xf, %rsp
    pushq $45
    jmp idt_common

idt_46:
    and $~0xf, %rsp
    pushq $46
    jmp idt_common

idt_47:
    and $~0xf, %rsp
    pushq $47
    jmp idt_common

idt_48:
    and $~0xf, %rsp
    pushq $48
    jmp idt_common

idt_49:
    and $~0xf, %rsp
    pushq $49
    jmp idt_common

idt_50:
    and $~0xf, %rsp
    pushq $50
    jmp idt_common

idt_51:
    and $~0xf, %rsp
    pushq $51
    jmp idt_common

idt_52:
    and $~0xf, %rsp
    pushq $52
    jmp idt_common

idt_53:
    and $~0xf, %rsp
    pushq $53
    jmp idt_common

idt_54:
    and $~0xf, %rsp
    pushq $54
    jmp idt_common

idt_55:
    and $~0xf, %rsp
    pushq $55
    jmp idt_common

idt_56:
    and $~0xf, %rsp
    pushq $56
    jmp idt_common

idt_57:
    and $~0xf, %rsp
    pushq $57
    jmp idt_common

idt_58:
    and $~0xf, %rsp
    pushq $58
    jmp idt_common

idt_59:
    and $~0xf, %rsp
    pushq $59
    jmp idt_common

idt_60:
    and $~0xf, %rsp
    pushq $60
    jmp idt_common

idt_61:
    and $~0xf, %rsp
    pushq $61
    jmp idt_common

idt_62:
    and $~0xf, %rsp
    pushq $62
    jmp idt_common

idt_63:
    and $~0xf, %rsp
    pushq $63
    jmp idt_common

idt_64:
    and $~0xf, %rsp
    pushq $64
    jmp idt_common

idt_65:
    and $~0xf, %rsp
    pushq $65
    jmp idt_common

idt_66:
    and $~0xf, %rsp
    pushq $66
    jmp idt_common

idt_67:
    and $~0xf, %rsp
    pushq $67
    jmp idt_common

idt_68:
    and $~0xf, %rsp
    pushq $68
    jmp idt_common

idt_69:
    and $~0xf, %rsp
    pushq $69
    jmp idt_common

idt_70:
    and $~0xf, %rsp
    pushq $70
    jmp idt_common

idt_71:
    and $~0xf, %rsp
    pushq $71
    jmp idt_common

idt_72:
    and $~0xf, %rsp
    pushq $72
    jmp idt_common

idt_73:
    and $~0xf, %rsp
    pushq $73
    jmp idt_common

idt_74:
    and $~0xf, %rsp
    pushq $74
    jmp idt_common

idt_75:
    and $~0xf, %rsp
    pushq $75
    jmp idt_common

idt_76:
    and $~0xf, %rsp
    pushq $76
    jmp idt_common

idt_77:
    and $~0xf, %rsp
    pushq $77
    jmp idt_common

idt_78:
    and $~0xf, %rsp
    pushq $78
    jmp idt_common

idt_79:
    and $~0xf, %rsp
    pushq $79
    jmp idt_common

idt_80:
    and $~0xf, %rsp
    pushq $80
    jmp idt_common

idt_81:
    and $~0xf, %rsp
    pushq $81
    jmp idt_common

idt_82:
    and $~0xf, %rsp
    pushq $82
    jmp idt_common

idt_83:
    and $~0xf, %rsp
    pushq $83
    jmp idt_common

idt_84:
    and $~0xf, %rsp
    pushq $84
    jmp idt_common

idt_85:
    and $~0xf, %rsp
    pushq $85
    jmp idt_common

idt_86:
    and $~0xf, %rsp
    pushq $86
    jmp idt_common

idt_87:
    and $~0xf, %rsp
    pushq $87
    jmp idt_common

idt_88:
    and $~0xf, %rsp
    pushq $88
    jmp idt_common

idt_89:
    and $~0xf, %rsp
    pushq $89
    jmp idt_common

idt_90:
    and $~0xf, %rsp
    pushq $90
    jmp idt_common

idt_91:
    and $~0xf, %rsp
    pushq $91
    jmp idt_common

idt_92:
    and $~0xf, %rsp
    pushq $92
    jmp idt_common

idt_93:
    and $~0xf, %rsp
    pushq $93
    jmp idt_common

idt_94:
    and $~0xf, %rsp
    pushq $94
    jmp idt_common

idt_95:
    and $~0xf, %rsp
    pushq $95
    jmp idt_common

idt_96:
    and $~0xf, %rsp
    pushq $96
    jmp idt_common

idt_97:
    and $~0xf, %rsp
    pushq $97
    jmp idt_common

idt_98:
    and $~0xf, %rsp
    pushq $98
    jmp idt_common

idt_99:
    and $~0xf, %rsp
    pushq $99
    jmp idt_common

idt_100:
    and $~0xf, %rsp
    pushq $100
    jmp idt_common

idt_101:
    and $~0xf, %rsp
    pushq $101
    jmp idt_common

idt_102:
    and $~0xf, %rsp
    pushq $102
    jmp idt_common

idt_103:
    and $~0xf, %rsp
    pushq $103
    jmp idt_common

idt_104:
    and $~0xf, %rsp
    pushq $104
    jmp idt_common

idt_105:
    and $~0xf, %rsp
    pushq $105
    jmp idt_common

idt_106:
    and $~0xf, %rsp
    pushq $106
    jmp idt_common

idt_107:
    and $~0xf, %rsp
    pushq $107
    jmp idt_common

idt_108:
    and $~0xf, %rsp
    pushq $108
    jmp idt_common

idt_109:
    and $~0xf, %rsp
    pushq $109
    jmp idt_common

idt_110:
    and $~0xf, %rsp
    pushq $110
    jmp idt_common

idt_111:
    and $~0xf, %rsp
    pushq $111
    jmp idt_common

idt_112:
    and $~0xf, %rsp
    pushq $112
    jmp idt_common

idt_113:
    and $~0xf, %rsp
    pushq $113
    jmp idt_common

idt_114:
    and $~0xf, %rsp
    pushq $114
    jmp idt_common

idt_115:
    and $~0xf, %rsp
    pushq $115
    jmp idt_common

idt_116:
    and $~0xf, %rsp
    pushq $116
    jmp idt_common

idt_117:
    and $~0xf, %rsp
    pushq $117
    jmp idt_common

idt_118:
    and $~0xf, %rsp
    pushq $118
    jmp idt_common

idt_119:
    and $~0xf, %rsp
    pushq $119
    jmp idt_common

idt_120:
    and $~0xf, %rsp
    pushq $120
    jmp idt_common

idt_121:
    and $~0xf, %rsp
    pushq $121
    jmp idt_common

idt_122:
    and $~0xf, %rsp
    pushq $122
    jmp idt_common

idt_123:
    and $~0xf, %rsp
    pushq $123
    jmp idt_common

idt_124:
    and $~0xf, %rsp
    pushq $124
    jmp idt_common

idt_125:
    and $~0xf, %rsp
    pushq $125
    jmp idt_common

idt_126:
    and $~0xf, %rsp
    pushq $126
    jmp idt_common

idt_127:
    and $~0xf, %rsp
    pushq $127
    jmp idt_common

idt_128:
    and $~0xf, %rsp
    pushq $128
    jmp idt_common

idt_129:
    and $~0xf, %rsp
    pushq $129
    jmp idt_common

idt_130:
    and $~0xf, %rsp
    pushq $130
    jmp idt_common

idt_131:
    and $~0xf, %rsp
    pushq $131
    jmp idt_common

idt_132:
    and $~0xf, %rsp
    pushq $132
    jmp idt_common

idt_133:
    and $~0xf, %rsp
    pushq $133
    jmp idt_common

idt_134:
    and $~0xf, %rsp
    pushq $134
    jmp idt_common

idt_135:
    and $~0xf, %rsp
    pushq $135
    jmp idt_common

idt_136:
    and $~0xf, %rsp
    pushq $136
    jmp idt_common

idt_137:
    and $~0xf, %rsp
    pushq $137
    jmp idt_common

idt_138:
    and $~0xf, %rsp
    pushq $138
    jmp idt_common

idt_139:
    and $~0xf, %rsp
    pushq $139
    jmp idt_common

idt_140:
    and $~0xf, %rsp
    pushq $140
    jmp idt_common

idt_141:
    and $~0xf, %rsp
    pushq $141
    jmp idt_common

idt_142:
    and $~0xf, %rsp
    pushq $142
    jmp idt_common

idt_143:
    and $~0xf, %rsp
    pushq $143
    jmp idt_common

idt_144:
    and $~0xf, %rsp
    pushq $144
    jmp idt_common

idt_145:
    and $~0xf, %rsp
    pushq $145
    jmp idt_common

idt_146:
    and $~0xf, %rsp
    pushq $146
    jmp idt_common

idt_147:
    and $~0xf, %rsp
    pushq $147
    jmp idt_common

idt_148:
    and $~0xf, %rsp
    pushq $148
    jmp idt_common

idt_149:
    and $~0xf, %rsp
    pushq $149
    jmp idt_common

idt_150:
    and $~0xf, %rsp
    pushq $150
    jmp idt_common

idt_151:
    and $~0xf, %rsp
    pushq $151
    jmp idt_common

idt_152:
    and $~0xf, %rsp
    pushq $152
    jmp idt_common

idt_153:
    and $~0xf, %rsp
    pushq $153
    jmp idt_common

idt_154:
    and $~0xf, %rsp
    pushq $154
    jmp idt_common

idt_155:
    and $~0xf, %rsp
    pushq $155
    jmp idt_common

idt_156:
    and $~0xf, %rsp
    pushq $156
    jmp idt_common

idt_157:
    and $~0xf, %rsp
    pushq $157
    jmp idt_common

idt_158:
    and $~0xf, %rsp
    pushq $158
    jmp idt_common

idt_159:
    and $~0xf, %rsp
    pushq $159
    jmp idt_common

idt_160:
    and $~0xf, %rsp
    pushq $160
    jmp idt_common

idt_161:
    and $~0xf, %rsp
    pushq $161
    jmp idt_common

idt_162:
    and $~0xf, %rsp
    pushq $162
    jmp idt_common

idt_163:
    and $~0xf, %rsp
    pushq $163
    jmp idt_common

idt_164:
    and $~0xf, %rsp
    pushq $164
    jmp idt_common

idt_165:
    and $~0xf, %rsp
    pushq $165
    jmp idt_common

idt_166:
    and $~0xf, %rsp
    pushq $166
    jmp idt_common

idt_167:
    and $~0xf, %rsp
    pushq $167
    jmp idt_common

idt_168:
    and $~0xf, %rsp
    pushq $168
    jmp idt_common

idt_169:
    and $~0xf, %rsp
    pushq $169
    jmp idt_common

idt_170:
    and $~0xf, %rsp
    pushq $170
    jmp idt_common

idt_171:
    and $~0xf, %rsp
    pushq $171
    jmp idt_common

idt_172:
    and $~0xf, %rsp
    pushq $172
    jmp idt_common

idt_173:
    and $~0xf, %rsp
    pushq $173
    jmp idt_common

idt_174:
    and $~0xf, %rsp
    pushq $174
    jmp idt_common

idt_175:
    and $~0xf, %rsp
    pushq $175
    jmp idt_common

idt_176:
    and $~0xf, %rsp
    pushq $176
    jmp idt_common

idt_177:
    and $~0xf, %rsp
    pushq $177
    jmp idt_common

idt_178:
    and $~0xf, %rsp
    pushq $178
    jmp idt_common

idt_179:
    and $~0xf, %rsp
    pushq $179
    jmp idt_common

idt_180:
    and $~0xf, %rsp
    pushq $180
    jmp idt_common

idt_181:
    and $~0xf, %rsp
    pushq $181
    jmp idt_common

idt_182:
    and $~0xf, %rsp
    pushq $182
    jmp idt_common

idt_183:
    and $~0xf, %rsp
    pushq $183
    jmp idt_common

idt_184:
    and $~0xf, %rsp
    pushq $184
    jmp idt_common

idt_185:
    and $~0xf, %rsp
    pushq $185
    jmp idt_common

idt_186:
    and $~0xf, %rsp
    pushq $186
    jmp idt_common

idt_187:
    and $~0xf, %rsp
    pushq $187
    jmp idt_common

idt_188:
    and $~0xf, %rsp
    pushq $188
    jmp idt_common

idt_189:
    and $~0xf, %rsp
    pushq $189
    jmp idt_common

idt_190:
    and $~0xf, %rsp
    pushq $190
    jmp idt_common

idt_191:
    and $~0xf, %rsp
    pushq $191
    jmp idt_common

idt_192:
    and $~0xf, %rsp
    pushq $192
    jmp idt_common

idt_193:
    and $~0xf, %rsp
    pushq $193
    jmp idt_common

idt_194:
    and $~0xf, %rsp
    pushq $194
    jmp idt_common

idt_195:
    and $~0xf, %rsp
    pushq $195
    jmp idt_common

idt_196:
    and $~0xf, %rsp
    pushq $196
    jmp idt_common

idt_197:
    and $~0xf, %rsp
    pushq $197
    jmp idt_common

idt_198:
    and $~0xf, %rsp
    pushq $198
    jmp idt_common

idt_199:
    and $~0xf, %rsp
    pushq $199
    jmp idt_common

idt_200:
    and $~0xf, %rsp
    pushq $200
    jmp idt_common

idt_201:
    and $~0xf, %rsp
    pushq $201
    jmp idt_common

idt_202:
    and $~0xf, %rsp
    pushq $202
    jmp idt_common

idt_203:
    and $~0xf, %rsp
    pushq $203
    jmp idt_common

idt_204:
    and $~0xf, %rsp
    pushq $204
    jmp idt_common

idt_205:
    and $~0xf, %rsp
    pushq $205
    jmp idt_common

idt_206:
    and $~0xf, %rsp
    pushq $206
    jmp idt_common

idt_207:
    and $~0xf, %rsp
    pushq $207
    jmp idt_common

idt_208:
    and $~0xf, %rsp
    pushq $208
    jmp idt_common

idt_209:
    and $~0xf, %rsp
    pushq $209
    jmp idt_common

idt_210:
    and $~0xf, %rsp
    pushq $210
    jmp idt_common

idt_211:
    and $~0xf, %rsp
    pushq $211
    jmp idt_common

idt_212:
    and $~0xf, %rsp
    pushq $212
    jmp idt_common

idt_213:
    and $~0xf, %rsp
    pushq $213
    jmp idt_common

idt_214:
    and $~0xf, %rsp
    pushq $214
    jmp idt_common

idt_215:
    and $~0xf, %rsp
    pushq $215
    jmp idt_common

idt_216:
    and $~0xf, %rsp
    pushq $216
    jmp idt_common

idt_217:
    and $~0xf, %rsp
    pushq $217
    jmp idt_common

idt_218:
    and $~0xf, %rsp
    pushq $218
    jmp idt_common

idt_219:
    and $~0xf, %rsp
    pushq $219
    jmp idt_common

idt_220:
    and $~0xf, %rsp
    pushq $220
    jmp idt_common

idt_221:
    and $~0xf, %rsp
    pushq $221
    jmp idt_common

idt_222:
    and $~0xf, %rsp
    pushq $222
    jmp idt_common

idt_223:
    and $~0xf, %rsp
    pushq $223
    jmp idt_common

idt_224:
    and $~0xf, %rsp
    pushq $224
    jmp idt_common

idt_225:
    and $~0xf, %rsp
    pushq $225
    jmp idt_common

idt_226:
    and $~0xf, %rsp
    pushq $226
    jmp idt_common

idt_227:
    and $~0xf, %rsp
    pushq $227
    jmp idt_common

idt_228:
    and $~0xf, %rsp
    pushq $228
    jmp idt_common

idt_229:
    and $~0xf, %rsp
    pushq $229
    jmp idt_common

idt_230:
    and $~0xf, %rsp
    pushq $230
    jmp idt_common

idt_231:
    and $~0xf, %rsp
    pushq $231
    jmp idt_common

idt_232:
    and $~0xf, %rsp
    pushq $232
    jmp idt_common

idt_233:
    and $~0xf, %rsp
    pushq $233
    jmp idt_common

idt_234:
    and $~0xf, %rsp
    pushq $234
    jmp idt_common

idt_235:
    and $~0xf, %rsp
    pushq $235
    jmp idt_common

idt_236:
    and $~0xf, %rsp
    pushq $236
    jmp idt_common

idt_237:
    and $~0xf, %rsp
    pushq $237
    jmp idt_common

idt_238:
    and $~0xf, %rsp
    pushq $238
    jmp idt_common

idt_239:
    and $~0xf, %rsp
    pushq $239
    jmp idt_common

idt_240:
    and $~0xf, %rsp
    pushq $240
    jmp idt_common

idt_241:
    and $~0xf, %rsp
    pushq $241
    jmp idt_common

idt_242:
    and $~0xf, %rsp
    pushq $242
    jmp idt_common

idt_243:
    and $~0xf, %rsp
    pushq $243
    jmp idt_common

idt_244:
    and $~0xf, %rsp
    pushq $244
    jmp idt_common

idt_245:
    and $~0xf, %rsp
    pushq $245
    jmp idt_common

idt_246:
    and $~0xf, %rsp
    pushq $246
    jmp idt_common

idt_247:
    and $~0xf, %rsp
    pushq $247
    jmp idt_common

idt_248:
    and $~0xf, %rsp
    pushq $248
    jmp idt_common

idt_249:
    and $~0xf, %rsp
    pushq $249
    jmp idt_common

idt_250:
    and $~0xf, %rsp
    pushq $250
    jmp idt_common

idt_251:
    and $~0xf, %rsp
    pushq $251
    jmp idt_common

idt_252:
    and $~0xf, %rsp
    pushq $252
    jmp idt_common

idt_253:
    and $~0xf, %rsp
    pushq $253
    jmp idt_common

idt_254:
    and $~0xf, %rsp
    pushq $254
    jmp idt_common

idt_255:
    and $~0xf, %rsp
    pushq $255
    jmp idt_common

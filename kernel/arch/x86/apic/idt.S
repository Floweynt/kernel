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


    // auto generated IDT handler stubs


idt_0:
    push $0
    and $~0xf, %rsp
    pushq $0
    jmp idt_common


idt_1:
    push $0
    and $~0xf, %rsp
    pushq $1
    jmp idt_common


idt_2:
    push $0
    and $~0xf, %rsp
    pushq $2
    jmp idt_common


idt_3:
    push $0
    and $~0xf, %rsp
    pushq $3
    jmp idt_common


idt_4:
    push $0
    and $~0xf, %rsp
    pushq $4
    jmp idt_common


idt_5:
    push $0
    and $~0xf, %rsp
    pushq $5
    jmp idt_common


idt_6:
    push $0
    and $~0xf, %rsp
    pushq $6
    jmp idt_common


idt_7:
    push $0
    and $~0xf, %rsp
    pushq $7
    jmp idt_common


idt_8:
    //push $0
    and $~0xf, %rsp
    pushq $8
    jmp idt_common


idt_9:
    push $0
    and $~0xf, %rsp
    pushq $9
    jmp idt_common


idt_10:
    //push $0
    and $~0xf, %rsp
    pushq $10
    jmp idt_common


idt_11:
    //push $0
    and $~0xf, %rsp
    pushq $11
    jmp idt_common


idt_12:
    //push $0
    and $~0xf, %rsp
    pushq $12
    jmp idt_common


idt_13:
    //push $0
    and $~0xf, %rsp
    pushq $13
    jmp idt_common


idt_14:
    //push $0
    and $~0xf, %rsp
    pushq $14
    jmp idt_common


idt_15:
    push $0
    and $~0xf, %rsp
    pushq $15
    jmp idt_common


idt_16:
    push $0
    and $~0xf, %rsp
    pushq $16
    jmp idt_common


idt_17:
    //push $0
    and $~0xf, %rsp
    pushq $17
    jmp idt_common


idt_18:
    push $0
    and $~0xf, %rsp
    pushq $18
    jmp idt_common


idt_19:
    push $0
    and $~0xf, %rsp
    pushq $19
    jmp idt_common


idt_20:
    push $0
    and $~0xf, %rsp
    pushq $20
    jmp idt_common


idt_21:
    push $0
    and $~0xf, %rsp
    pushq $21
    jmp idt_common


idt_22:
    push $0
    and $~0xf, %rsp
    pushq $22
    jmp idt_common


idt_23:
    push $0
    and $~0xf, %rsp
    pushq $23
    jmp idt_common


idt_24:
    push $0
    and $~0xf, %rsp
    pushq $24
    jmp idt_common


idt_25:
    push $0
    and $~0xf, %rsp
    pushq $25
    jmp idt_common


idt_26:
    push $0
    and $~0xf, %rsp
    pushq $26
    jmp idt_common


idt_27:
    push $0
    and $~0xf, %rsp
    pushq $27
    jmp idt_common


idt_28:
    push $0
    and $~0xf, %rsp
    pushq $28
    jmp idt_common


idt_29:
    push $0
    and $~0xf, %rsp
    pushq $29
    jmp idt_common


idt_30:
    push $0
    and $~0xf, %rsp
    pushq $30
    jmp idt_common


idt_31:
    push $0
    and $~0xf, %rsp
    pushq $31
    jmp idt_common


idt_32:
    push $0
    and $~0xf, %rsp
    pushq $32
    jmp idt_common


idt_33:
    push $0
    and $~0xf, %rsp
    pushq $33
    jmp idt_common


idt_34:
    push $0
    and $~0xf, %rsp
    pushq $34
    jmp idt_common


idt_35:
    push $0
    and $~0xf, %rsp
    pushq $35
    jmp idt_common


idt_36:
    push $0
    and $~0xf, %rsp
    pushq $36
    jmp idt_common


idt_37:
    push $0
    and $~0xf, %rsp
    pushq $37
    jmp idt_common


idt_38:
    push $0
    and $~0xf, %rsp
    pushq $38
    jmp idt_common


idt_39:
    push $0
    and $~0xf, %rsp
    pushq $39
    jmp idt_common


idt_40:
    push $0
    and $~0xf, %rsp
    pushq $40
    jmp idt_common


idt_41:
    push $0
    and $~0xf, %rsp
    pushq $41
    jmp idt_common


idt_42:
    push $0
    and $~0xf, %rsp
    pushq $42
    jmp idt_common


idt_43:
    push $0
    and $~0xf, %rsp
    pushq $43
    jmp idt_common


idt_44:
    push $0
    and $~0xf, %rsp
    pushq $44
    jmp idt_common


idt_45:
    push $0
    and $~0xf, %rsp
    pushq $45
    jmp idt_common


idt_46:
    push $0
    and $~0xf, %rsp
    pushq $46
    jmp idt_common


idt_47:
    push $0
    and $~0xf, %rsp
    pushq $47
    jmp idt_common


idt_48:
    push $0
    and $~0xf, %rsp
    pushq $48
    jmp idt_common


idt_49:
    push $0
    and $~0xf, %rsp
    pushq $49
    jmp idt_common


idt_50:
    push $0
    and $~0xf, %rsp
    pushq $50
    jmp idt_common


idt_51:
    push $0
    and $~0xf, %rsp
    pushq $51
    jmp idt_common


idt_52:
    push $0
    and $~0xf, %rsp
    pushq $52
    jmp idt_common


idt_53:
    push $0
    and $~0xf, %rsp
    pushq $53
    jmp idt_common


idt_54:
    push $0
    and $~0xf, %rsp
    pushq $54
    jmp idt_common


idt_55:
    push $0
    and $~0xf, %rsp
    pushq $55
    jmp idt_common


idt_56:
    push $0
    and $~0xf, %rsp
    pushq $56
    jmp idt_common


idt_57:
    push $0
    and $~0xf, %rsp
    pushq $57
    jmp idt_common


idt_58:
    push $0
    and $~0xf, %rsp
    pushq $58
    jmp idt_common


idt_59:
    push $0
    and $~0xf, %rsp
    pushq $59
    jmp idt_common


idt_60:
    push $0
    and $~0xf, %rsp
    pushq $60
    jmp idt_common


idt_61:
    push $0
    and $~0xf, %rsp
    pushq $61
    jmp idt_common


idt_62:
    push $0
    and $~0xf, %rsp
    pushq $62
    jmp idt_common


idt_63:
    push $0
    and $~0xf, %rsp
    pushq $63
    jmp idt_common


idt_64:
    push $0
    and $~0xf, %rsp
    pushq $64
    jmp idt_common


idt_65:
    push $0
    and $~0xf, %rsp
    pushq $65
    jmp idt_common


idt_66:
    push $0
    and $~0xf, %rsp
    pushq $66
    jmp idt_common


idt_67:
    push $0
    and $~0xf, %rsp
    pushq $67
    jmp idt_common


idt_68:
    push $0
    and $~0xf, %rsp
    pushq $68
    jmp idt_common


idt_69:
    push $0
    and $~0xf, %rsp
    pushq $69
    jmp idt_common


idt_70:
    push $0
    and $~0xf, %rsp
    pushq $70
    jmp idt_common


idt_71:
    push $0
    and $~0xf, %rsp
    pushq $71
    jmp idt_common


idt_72:
    push $0
    and $~0xf, %rsp
    pushq $72
    jmp idt_common


idt_73:
    push $0
    and $~0xf, %rsp
    pushq $73
    jmp idt_common


idt_74:
    push $0
    and $~0xf, %rsp
    pushq $74
    jmp idt_common


idt_75:
    push $0
    and $~0xf, %rsp
    pushq $75
    jmp idt_common


idt_76:
    push $0
    and $~0xf, %rsp
    pushq $76
    jmp idt_common


idt_77:
    push $0
    and $~0xf, %rsp
    pushq $77
    jmp idt_common


idt_78:
    push $0
    and $~0xf, %rsp
    pushq $78
    jmp idt_common


idt_79:
    push $0
    and $~0xf, %rsp
    pushq $79
    jmp idt_common


idt_80:
    push $0
    and $~0xf, %rsp
    pushq $80
    jmp idt_common


idt_81:
    push $0
    and $~0xf, %rsp
    pushq $81
    jmp idt_common


idt_82:
    push $0
    and $~0xf, %rsp
    pushq $82
    jmp idt_common


idt_83:
    push $0
    and $~0xf, %rsp
    pushq $83
    jmp idt_common


idt_84:
    push $0
    and $~0xf, %rsp
    pushq $84
    jmp idt_common


idt_85:
    push $0
    and $~0xf, %rsp
    pushq $85
    jmp idt_common


idt_86:
    push $0
    and $~0xf, %rsp
    pushq $86
    jmp idt_common


idt_87:
    push $0
    and $~0xf, %rsp
    pushq $87
    jmp idt_common


idt_88:
    push $0
    and $~0xf, %rsp
    pushq $88
    jmp idt_common


idt_89:
    push $0
    and $~0xf, %rsp
    pushq $89
    jmp idt_common


idt_90:
    push $0
    and $~0xf, %rsp
    pushq $90
    jmp idt_common


idt_91:
    push $0
    and $~0xf, %rsp
    pushq $91
    jmp idt_common


idt_92:
    push $0
    and $~0xf, %rsp
    pushq $92
    jmp idt_common


idt_93:
    push $0
    and $~0xf, %rsp
    pushq $93
    jmp idt_common


idt_94:
    push $0
    and $~0xf, %rsp
    pushq $94
    jmp idt_common


idt_95:
    push $0
    and $~0xf, %rsp
    pushq $95
    jmp idt_common


idt_96:
    push $0
    and $~0xf, %rsp
    pushq $96
    jmp idt_common


idt_97:
    push $0
    and $~0xf, %rsp
    pushq $97
    jmp idt_common


idt_98:
    push $0
    and $~0xf, %rsp
    pushq $98
    jmp idt_common


idt_99:
    push $0
    and $~0xf, %rsp
    pushq $99
    jmp idt_common


idt_100:
    push $0
    and $~0xf, %rsp
    pushq $100
    jmp idt_common


idt_101:
    push $0
    and $~0xf, %rsp
    pushq $101
    jmp idt_common


idt_102:
    push $0
    and $~0xf, %rsp
    pushq $102
    jmp idt_common


idt_103:
    push $0
    and $~0xf, %rsp
    pushq $103
    jmp idt_common


idt_104:
    push $0
    and $~0xf, %rsp
    pushq $104
    jmp idt_common


idt_105:
    push $0
    and $~0xf, %rsp
    pushq $105
    jmp idt_common


idt_106:
    push $0
    and $~0xf, %rsp
    pushq $106
    jmp idt_common


idt_107:
    push $0
    and $~0xf, %rsp
    pushq $107
    jmp idt_common


idt_108:
    push $0
    and $~0xf, %rsp
    pushq $108
    jmp idt_common


idt_109:
    push $0
    and $~0xf, %rsp
    pushq $109
    jmp idt_common


idt_110:
    push $0
    and $~0xf, %rsp
    pushq $110
    jmp idt_common


idt_111:
    push $0
    and $~0xf, %rsp
    pushq $111
    jmp idt_common


idt_112:
    push $0
    and $~0xf, %rsp
    pushq $112
    jmp idt_common


idt_113:
    push $0
    and $~0xf, %rsp
    pushq $113
    jmp idt_common


idt_114:
    push $0
    and $~0xf, %rsp
    pushq $114
    jmp idt_common


idt_115:
    push $0
    and $~0xf, %rsp
    pushq $115
    jmp idt_common


idt_116:
    push $0
    and $~0xf, %rsp
    pushq $116
    jmp idt_common


idt_117:
    push $0
    and $~0xf, %rsp
    pushq $117
    jmp idt_common


idt_118:
    push $0
    and $~0xf, %rsp
    pushq $118
    jmp idt_common


idt_119:
    push $0
    and $~0xf, %rsp
    pushq $119
    jmp idt_common


idt_120:
    push $0
    and $~0xf, %rsp
    pushq $120
    jmp idt_common


idt_121:
    push $0
    and $~0xf, %rsp
    pushq $121
    jmp idt_common


idt_122:
    push $0
    and $~0xf, %rsp
    pushq $122
    jmp idt_common


idt_123:
    push $0
    and $~0xf, %rsp
    pushq $123
    jmp idt_common


idt_124:
    push $0
    and $~0xf, %rsp
    pushq $124
    jmp idt_common


idt_125:
    push $0
    and $~0xf, %rsp
    pushq $125
    jmp idt_common


idt_126:
    push $0
    and $~0xf, %rsp
    pushq $126
    jmp idt_common


idt_127:
    push $0
    and $~0xf, %rsp
    pushq $127
    jmp idt_common


idt_128:
    push $0
    and $~0xf, %rsp
    pushq $128
    jmp idt_common


idt_129:
    push $0
    and $~0xf, %rsp
    pushq $129
    jmp idt_common


idt_130:
    push $0
    and $~0xf, %rsp
    pushq $130
    jmp idt_common


idt_131:
    push $0
    and $~0xf, %rsp
    pushq $131
    jmp idt_common


idt_132:
    push $0
    and $~0xf, %rsp
    pushq $132
    jmp idt_common


idt_133:
    push $0
    and $~0xf, %rsp
    pushq $133
    jmp idt_common


idt_134:
    push $0
    and $~0xf, %rsp
    pushq $134
    jmp idt_common


idt_135:
    push $0
    and $~0xf, %rsp
    pushq $135
    jmp idt_common


idt_136:
    push $0
    and $~0xf, %rsp
    pushq $136
    jmp idt_common


idt_137:
    push $0
    and $~0xf, %rsp
    pushq $137
    jmp idt_common


idt_138:
    push $0
    and $~0xf, %rsp
    pushq $138
    jmp idt_common


idt_139:
    push $0
    and $~0xf, %rsp
    pushq $139
    jmp idt_common


idt_140:
    push $0
    and $~0xf, %rsp
    pushq $140
    jmp idt_common


idt_141:
    push $0
    and $~0xf, %rsp
    pushq $141
    jmp idt_common


idt_142:
    push $0
    and $~0xf, %rsp
    pushq $142
    jmp idt_common


idt_143:
    push $0
    and $~0xf, %rsp
    pushq $143
    jmp idt_common


idt_144:
    push $0
    and $~0xf, %rsp
    pushq $144
    jmp idt_common


idt_145:
    push $0
    and $~0xf, %rsp
    pushq $145
    jmp idt_common


idt_146:
    push $0
    and $~0xf, %rsp
    pushq $146
    jmp idt_common


idt_147:
    push $0
    and $~0xf, %rsp
    pushq $147
    jmp idt_common


idt_148:
    push $0
    and $~0xf, %rsp
    pushq $148
    jmp idt_common


idt_149:
    push $0
    and $~0xf, %rsp
    pushq $149
    jmp idt_common


idt_150:
    push $0
    and $~0xf, %rsp
    pushq $150
    jmp idt_common


idt_151:
    push $0
    and $~0xf, %rsp
    pushq $151
    jmp idt_common


idt_152:
    push $0
    and $~0xf, %rsp
    pushq $152
    jmp idt_common


idt_153:
    push $0
    and $~0xf, %rsp
    pushq $153
    jmp idt_common


idt_154:
    push $0
    and $~0xf, %rsp
    pushq $154
    jmp idt_common


idt_155:
    push $0
    and $~0xf, %rsp
    pushq $155
    jmp idt_common


idt_156:
    push $0
    and $~0xf, %rsp
    pushq $156
    jmp idt_common


idt_157:
    push $0
    and $~0xf, %rsp
    pushq $157
    jmp idt_common


idt_158:
    push $0
    and $~0xf, %rsp
    pushq $158
    jmp idt_common


idt_159:
    push $0
    and $~0xf, %rsp
    pushq $159
    jmp idt_common


idt_160:
    push $0
    and $~0xf, %rsp
    pushq $160
    jmp idt_common


idt_161:
    push $0
    and $~0xf, %rsp
    pushq $161
    jmp idt_common


idt_162:
    push $0
    and $~0xf, %rsp
    pushq $162
    jmp idt_common


idt_163:
    push $0
    and $~0xf, %rsp
    pushq $163
    jmp idt_common


idt_164:
    push $0
    and $~0xf, %rsp
    pushq $164
    jmp idt_common


idt_165:
    push $0
    and $~0xf, %rsp
    pushq $165
    jmp idt_common


idt_166:
    push $0
    and $~0xf, %rsp
    pushq $166
    jmp idt_common


idt_167:
    push $0
    and $~0xf, %rsp
    pushq $167
    jmp idt_common


idt_168:
    push $0
    and $~0xf, %rsp
    pushq $168
    jmp idt_common


idt_169:
    push $0
    and $~0xf, %rsp
    pushq $169
    jmp idt_common


idt_170:
    push $0
    and $~0xf, %rsp
    pushq $170
    jmp idt_common


idt_171:
    push $0
    and $~0xf, %rsp
    pushq $171
    jmp idt_common


idt_172:
    push $0
    and $~0xf, %rsp
    pushq $172
    jmp idt_common


idt_173:
    push $0
    and $~0xf, %rsp
    pushq $173
    jmp idt_common


idt_174:
    push $0
    and $~0xf, %rsp
    pushq $174
    jmp idt_common


idt_175:
    push $0
    and $~0xf, %rsp
    pushq $175
    jmp idt_common


idt_176:
    push $0
    and $~0xf, %rsp
    pushq $176
    jmp idt_common


idt_177:
    push $0
    and $~0xf, %rsp
    pushq $177
    jmp idt_common


idt_178:
    push $0
    and $~0xf, %rsp
    pushq $178
    jmp idt_common


idt_179:
    push $0
    and $~0xf, %rsp
    pushq $179
    jmp idt_common


idt_180:
    push $0
    and $~0xf, %rsp
    pushq $180
    jmp idt_common


idt_181:
    push $0
    and $~0xf, %rsp
    pushq $181
    jmp idt_common


idt_182:
    push $0
    and $~0xf, %rsp
    pushq $182
    jmp idt_common


idt_183:
    push $0
    and $~0xf, %rsp
    pushq $183
    jmp idt_common


idt_184:
    push $0
    and $~0xf, %rsp
    pushq $184
    jmp idt_common


idt_185:
    push $0
    and $~0xf, %rsp
    pushq $185
    jmp idt_common


idt_186:
    push $0
    and $~0xf, %rsp
    pushq $186
    jmp idt_common


idt_187:
    push $0
    and $~0xf, %rsp
    pushq $187
    jmp idt_common


idt_188:
    push $0
    and $~0xf, %rsp
    pushq $188
    jmp idt_common


idt_189:
    push $0
    and $~0xf, %rsp
    pushq $189
    jmp idt_common


idt_190:
    push $0
    and $~0xf, %rsp
    pushq $190
    jmp idt_common


idt_191:
    push $0
    and $~0xf, %rsp
    pushq $191
    jmp idt_common


idt_192:
    push $0
    and $~0xf, %rsp
    pushq $192
    jmp idt_common


idt_193:
    push $0
    and $~0xf, %rsp
    pushq $193
    jmp idt_common


idt_194:
    push $0
    and $~0xf, %rsp
    pushq $194
    jmp idt_common


idt_195:
    push $0
    and $~0xf, %rsp
    pushq $195
    jmp idt_common


idt_196:
    push $0
    and $~0xf, %rsp
    pushq $196
    jmp idt_common


idt_197:
    push $0
    and $~0xf, %rsp
    pushq $197
    jmp idt_common


idt_198:
    push $0
    and $~0xf, %rsp
    pushq $198
    jmp idt_common


idt_199:
    push $0
    and $~0xf, %rsp
    pushq $199
    jmp idt_common


idt_200:
    push $0
    and $~0xf, %rsp
    pushq $200
    jmp idt_common


idt_201:
    push $0
    and $~0xf, %rsp
    pushq $201
    jmp idt_common


idt_202:
    push $0
    and $~0xf, %rsp
    pushq $202
    jmp idt_common


idt_203:
    push $0
    and $~0xf, %rsp
    pushq $203
    jmp idt_common


idt_204:
    push $0
    and $~0xf, %rsp
    pushq $204
    jmp idt_common


idt_205:
    push $0
    and $~0xf, %rsp
    pushq $205
    jmp idt_common


idt_206:
    push $0
    and $~0xf, %rsp
    pushq $206
    jmp idt_common


idt_207:
    push $0
    and $~0xf, %rsp
    pushq $207
    jmp idt_common


idt_208:
    push $0
    and $~0xf, %rsp
    pushq $208
    jmp idt_common


idt_209:
    push $0
    and $~0xf, %rsp
    pushq $209
    jmp idt_common


idt_210:
    push $0
    and $~0xf, %rsp
    pushq $210
    jmp idt_common


idt_211:
    push $0
    and $~0xf, %rsp
    pushq $211
    jmp idt_common


idt_212:
    push $0
    and $~0xf, %rsp
    pushq $212
    jmp idt_common


idt_213:
    push $0
    and $~0xf, %rsp
    pushq $213
    jmp idt_common


idt_214:
    push $0
    and $~0xf, %rsp
    pushq $214
    jmp idt_common


idt_215:
    push $0
    and $~0xf, %rsp
    pushq $215
    jmp idt_common


idt_216:
    push $0
    and $~0xf, %rsp
    pushq $216
    jmp idt_common


idt_217:
    push $0
    and $~0xf, %rsp
    pushq $217
    jmp idt_common


idt_218:
    push $0
    and $~0xf, %rsp
    pushq $218
    jmp idt_common


idt_219:
    push $0
    and $~0xf, %rsp
    pushq $219
    jmp idt_common


idt_220:
    push $0
    and $~0xf, %rsp
    pushq $220
    jmp idt_common


idt_221:
    push $0
    and $~0xf, %rsp
    pushq $221
    jmp idt_common


idt_222:
    push $0
    and $~0xf, %rsp
    pushq $222
    jmp idt_common


idt_223:
    push $0
    and $~0xf, %rsp
    pushq $223
    jmp idt_common


idt_224:
    push $0
    and $~0xf, %rsp
    pushq $224
    jmp idt_common


idt_225:
    push $0
    and $~0xf, %rsp
    pushq $225
    jmp idt_common


idt_226:
    push $0
    and $~0xf, %rsp
    pushq $226
    jmp idt_common


idt_227:
    push $0
    and $~0xf, %rsp
    pushq $227
    jmp idt_common


idt_228:
    push $0
    and $~0xf, %rsp
    pushq $228
    jmp idt_common


idt_229:
    push $0
    and $~0xf, %rsp
    pushq $229
    jmp idt_common


idt_230:
    push $0
    and $~0xf, %rsp
    pushq $230
    jmp idt_common


idt_231:
    push $0
    and $~0xf, %rsp
    pushq $231
    jmp idt_common


idt_232:
    push $0
    and $~0xf, %rsp
    pushq $232
    jmp idt_common


idt_233:
    push $0
    and $~0xf, %rsp
    pushq $233
    jmp idt_common


idt_234:
    push $0
    and $~0xf, %rsp
    pushq $234
    jmp idt_common


idt_235:
    push $0
    and $~0xf, %rsp
    pushq $235
    jmp idt_common


idt_236:
    push $0
    and $~0xf, %rsp
    pushq $236
    jmp idt_common


idt_237:
    push $0
    and $~0xf, %rsp
    pushq $237
    jmp idt_common


idt_238:
    push $0
    and $~0xf, %rsp
    pushq $238
    jmp idt_common


idt_239:
    push $0
    and $~0xf, %rsp
    pushq $239
    jmp idt_common


idt_240:
    push $0
    and $~0xf, %rsp
    pushq $240
    jmp idt_common


idt_241:
    push $0
    and $~0xf, %rsp
    pushq $241
    jmp idt_common


idt_242:
    push $0
    and $~0xf, %rsp
    pushq $242
    jmp idt_common


idt_243:
    push $0
    and $~0xf, %rsp
    pushq $243
    jmp idt_common


idt_244:
    push $0
    and $~0xf, %rsp
    pushq $244
    jmp idt_common


idt_245:
    push $0
    and $~0xf, %rsp
    pushq $245
    jmp idt_common


idt_246:
    push $0
    and $~0xf, %rsp
    pushq $246
    jmp idt_common


idt_247:
    push $0
    and $~0xf, %rsp
    pushq $247
    jmp idt_common


idt_248:
    push $0
    and $~0xf, %rsp
    pushq $248
    jmp idt_common


idt_249:
    push $0
    and $~0xf, %rsp
    pushq $249
    jmp idt_common


idt_250:
    push $0
    and $~0xf, %rsp
    pushq $250
    jmp idt_common


idt_251:
    push $0
    and $~0xf, %rsp
    pushq $251
    jmp idt_common


idt_252:
    push $0
    and $~0xf, %rsp
    pushq $252
    jmp idt_common


idt_253:
    push $0
    and $~0xf, %rsp
    pushq $253
    jmp idt_common


idt_254:
    push $0
    and $~0xf, %rsp
    pushq $254
    jmp idt_common


idt_255:
    push $0
    and $~0xf, %rsp
    pushq $255
    jmp idt_common


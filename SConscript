Import('RTT_ROOT')
Import('rtconfig')
from building import *

src = Glob('*.c')
CPPPATH = [RTT_ROOT + '/components/coap']
group = DefineGroup('coap', src, depend = ['RT_USING_COAP'], CPPPATH = CPPPATH)

Return('group')

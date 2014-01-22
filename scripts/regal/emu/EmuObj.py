#!/usr/bin/python -B

objFormulae = {
    'options' : {
      'originate' : [
        'glGenBuffers',
        'glDeleteBuffers',
        'glGenVertexArrays',
        'glDeleteVertexArrays',
        'glGenTextures',
        'glDeleteTextures',
      ],
    },
    'Buffers' : {
        'entries' : [ 'gl(GenBuffers|DeleteBuffers|BindBuffer)(ARB|)' ],
        'impl' : [ '_context->obj->${m1}(*_context, ${arg0plus}); return;', ],
    },
    'VAOs' : {
        'entries' : [ 'gl(GenVertexArrays|DeleteVertexArrays|BindVertexArray)(ARB|)' ],
        'impl' : [ '_context->obj->${m1}(*_context, ${arg0plus}); return;', ],
    },
    'Textures' : {
        'entries' : [ 'gl(GenTextures|DeleteTextures|BindTexture)' ],
        'impl' : [ '_context->obj->${m1}(*_context, ${arg0plus}); return;', ],
    },
    'FramebufferTexture' : {
        'entries' : [ 'glFramebufferTexture(1D|1DEXT|2D|2DEXT|2DOES|2DMultisampleEXT|2DMultisampleOES|3DEXT|3DOES)' ],
        'impl' : ['orig.glFramebufferTexture${m1}( _context,${arg0}, ${arg1}, ${arg2}, _context->obj->textureNames.ToDriverName(${arg3}), ${arg4plus}); return;', ],
     },
    'GetFramebufferAttachmentParameteriv' : {
        'entries' : [ 'glGetFramebufferAttachmentParameteriv(EXT|)' ],
        'impl' : [
            'orig.glGetFramebufferAttachmentParameteriv${m1}( _context,${arg0plus});',
            'if (${arg2} == GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME)',
            '{',
            '  GLint attachType = GL_RENDERBUFFER;',
            '  orig.glGetFramebufferAttachmentParameteriv${m1}( _context,${arg0}, ${arg1}, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &attachType);',
            '  if (attachType == GL_TEXTURE)',
            '    *${arg3} = _context->obj->textureNames.ToAppName(*${arg3});',
            '}',
            'return;',
        ],
    },
    'IsObj' : {
        'entries' : [ 'glIs(Buffer|VertexArray|Texture)(ARB|)' ],
        'impl' : [ 'return _context->obj->Is${m1}(*_context, ${arg0plus});', ],
    },
}
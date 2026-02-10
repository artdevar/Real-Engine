#pragma once

#include <glad/glad.h>
#include <vector>
#include "utils/Common.h"

template <class Derived>
class CBuffer
{
  DISABLE_CLASS_COPY(CBuffer);

public:
  CBuffer(CBuffer &&_Other) noexcept : m_ID(_Other.m_ID)
  {
    _Other.m_ID = INVALID_BUFFER;
  }

  CBuffer &operator=(CBuffer &&_Other)
  {
    if (m_ID != _Other.m_ID)
      Shutdown();

    m_ID = _Other.m_ID;
    _Other.m_ID = INVALID_BUFFER;

    return *this;
  }

  ~CBuffer()
  {
    Shutdown();
  }

  void Bind()
  {
    assert(m_ID != INVALID_BUFFER);
    static_cast<Derived *>(this)->BindBuffer();
  }

  void Unbind()
  {
    static_cast<Derived *>(this)->UnbindBuffer();
  }

  GLuint ID() const
  {
    return m_ID;
  }

protected:
  CBuffer() : m_ID(INVALID_BUFFER)
  {
    Init();
  }

  void Init()
  {
    assert(m_ID == INVALID_BUFFER);
    static_cast<Derived *>(this)->GenerateBuffer();
    assert(m_ID != INVALID_BUFFER);
  }

  void Shutdown()
  {
    if (m_ID != INVALID_BUFFER)
    {
      static_cast<Derived *>(this)->DeleteBuffer();
      m_ID = INVALID_BUFFER;
    }
  }

  static constexpr inline GLuint INVALID_BUFFER = 0u;

  GLuint m_ID;
};

// ------------------------------------------------

class CVertexArray final : public CBuffer<CVertexArray>
{
  friend CBuffer;

public:
  void EnableAttrib(GLuint _Index, GLint _Size, GLenum _Type, GLsizei _Stride, const void *_Offset = (GLvoid *)0)
  {
    glEnableVertexArrayAttrib(m_ID, _Index);

    switch (_Type)
    {
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
    case GL_INT:
    case GL_UNSIGNED_INT:
      glVertexAttribIPointer(_Index, _Size, _Type, _Stride, _Offset);
      break;

    case GL_DOUBLE:
      glVertexAttribLPointer(_Index, _Size, _Type, _Stride, _Offset);

    default:
      glVertexAttribPointer(_Index, _Size, _Type, GL_FALSE, _Stride, _Offset);
      break;
    }
  }

  void EnableAttribWithDivisor(GLuint _Index, GLint _Size, GLenum _Type, GLsizei _Stride, const void *_Offset, GLuint _Divisor)
  {
    EnableAttrib(_Index, _Size, _Type, _Stride, _Offset);
    glVertexAttribDivisor(_Index, _Divisor);
  }

protected:
  void GenerateBuffer()
  {
    glGenVertexArrays(1, &m_ID);
  }

  void BindBuffer()
  {
    glBindVertexArray(m_ID);
  }

  void UnbindBuffer()
  {
    glBindVertexArray(INVALID_BUFFER);
  }

  void DeleteBuffer()
  {
    glDeleteVertexArrays(1, &m_ID);
  }
};

// ------------------------------------------------

class CFrameBuffer final : public CBuffer<CFrameBuffer>
{
  friend CBuffer;

public:
  void AttachTexture(GLenum _Attachment, GLuint _TextureID)
  {
    glFramebufferTexture2D(GL_FRAMEBUFFER, _Attachment, GL_TEXTURE_2D, _TextureID, 0);
  }

  void AttachRenderbuffer(GLenum _Attachment, GLuint _RenderbufferID)
  {
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, _Attachment, GL_RENDERBUFFER, _RenderbufferID);
  }

  void DisableColorBuffer()
  {
    glNamedFramebufferDrawBuffer(m_ID, GL_NONE);
    glNamedFramebufferReadBuffer(m_ID, GL_NONE);
  }

protected:
  void GenerateBuffer()
  {
    glGenFramebuffers(1, &m_ID);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
  }

  void BindBuffer()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
  }

  void UnbindBuffer()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, INVALID_BUFFER);
  }

  void DeleteBuffer()
  {
    glDeleteFramebuffers(1, &m_ID);
  }
};

// ------------------------------------------------

class CBufferObject : public CBuffer<CBufferObject>
{
  friend CBuffer;

public:
  CBufferObject(GLenum _Target, GLenum _Usage) : m_Target(_Target),
                                                 m_Usage(_Usage),
                                                 m_Capacity(0),
                                                 m_ActualSize(0)
  {
  }

  CBufferObject(CBufferObject &&_Other) noexcept : CBuffer<CBufferObject>(std::move(_Other)),
                                                   m_Target(_Other.m_Target),
                                                   m_Usage(_Other.m_Usage),
                                                   m_Capacity(_Other.m_Capacity),
                                                   m_ActualSize(_Other.m_ActualSize)
  {
    _Other.m_Target = 0;
    _Other.m_Usage = 0;
    _Other.m_Capacity = 0;
    _Other.m_ActualSize = 0;
  }

  CBufferObject &operator=(CBufferObject &&_Other)
  {
    if (m_ID != _Other.m_ID)
      Shutdown();

    m_ID = _Other.m_ID;
    m_Target = _Other.m_Target;
    m_Usage = _Other.m_Usage;
    m_Capacity = _Other.m_Capacity;
    m_ActualSize = _Other.m_ActualSize;
    _Other.m_ID = INVALID_BUFFER;
    _Other.m_Target = 0;
    _Other.m_Usage = 0;
    _Other.m_Capacity = 0;
    _Other.m_ActualSize = 0;

    return *this;
  }

  void Clear()
  {
    if (m_ActualSize == 0)
      return;

    glInvalidateBufferData(m_ID);
    m_ActualSize = 0;
  }

  template <typename T>
    requires(std::is_trivially_copyable_v<T>)
  void Assign(T *_Data, GLsizeiptr _DataSizeInBytes)
  {
    assert(_DataSizeInBytes > 0);

    if (m_Capacity == _DataSizeInBytes)
      glNamedBufferSubData(m_ID, 0, _DataSizeInBytes, _Data);
    else
      glNamedBufferData(m_ID, _DataSizeInBytes, _Data, m_Usage);

    m_Capacity = _DataSizeInBytes;
    m_ActualSize = _DataSizeInBytes;
  }

  template <typename T>
  void Assign(const std::vector<T> &_Data)
  {
    Assign(_Data.data(), _Data.size() * sizeof(T));
  }

  template <typename T>
    requires(std::is_trivially_copyable_v<T>)
  void Push(T *_Data, GLsizeiptr _DataSizeInBytes)
  {
    assert(_DataSizeInBytes > 0);

    if (m_ActualSize + _DataSizeInBytes > m_Capacity)
      Reallocate(m_ActualSize + _DataSizeInBytes);

    glNamedBufferSubData(m_ID, m_ActualSize, _DataSizeInBytes, _Data);
    m_ActualSize += _DataSizeInBytes;
  }

  template <typename T>
  void Push(const std::vector<T> &_Data)
  {
    Push(_Data.data(), _Data.size() * sizeof(T));
  }

  void Erase(GLintptr _Offset, GLsizeiptr _SizeInBytes)
  {
    GLuint NewBufferID = 0;
    glGenBuffers(1, &NewBufferID);
    glBindBuffer(m_Target, NewBufferID);
    glBufferData(m_Target, m_Capacity - _SizeInBytes, nullptr, m_Usage);

    glCopyNamedBufferSubData(m_ID, NewBufferID, 0, 0, _Offset);
    glCopyNamedBufferSubData(m_ID, NewBufferID, _Offset + _SizeInBytes, _Offset, m_ActualSize - _Offset - _SizeInBytes);
    glDeleteBuffers(1, &m_ID);
    m_ID = NewBufferID;

    m_Capacity -= _SizeInBytes;
    m_ActualSize -= _SizeInBytes;
    assert(m_Capacity >= 0 && m_ActualSize >= 0);
  }

  void BindToTarget(GLenum _Target)
  {
    glBindBuffer(_Target, m_ID);
  }

  void BindToBase(GLuint _Index)
  {
    assert(m_Target == GL_ATOMIC_COUNTER_BUFFER ||
           m_Target == GL_TRANSFORM_FEEDBACK_BUFFER ||
           m_Target == GL_UNIFORM_BUFFER ||
           m_Target == GL_SHADER_STORAGE_BUFFER && "Wrong target");

    glBindBufferBase(m_Target, _Index, m_ID);
  }

  void Reserve(GLsizeiptr _SizeInBytes)
  {
    if (_SizeInBytes <= m_Capacity)
      return;

    ReallocateImpl(_SizeInBytes, m_ActualSize != 0);
  }

  void Shrink()
  {
    if (m_ActualSize == m_Capacity)
      return;

    assert(m_ActualSize < m_Capacity);
    ReallocateImpl(m_ActualSize, m_ActualSize != 0);
  }

  GLsizeiptr GetCapacity() const
  {
    return m_Capacity;
  }

  GLsizeiptr GetSize() const
  {
    return m_ActualSize;
  }

protected:
  void Reallocate(GLsizeiptr _RequiredSize)
  {
    const bool NeedCopyData = m_ActualSize != 0;
    const GLsizeiptr NewBufferCapacity = static_cast<GLsizeiptr>(_RequiredSize * 1.3f);

    ReallocateImpl(NewBufferCapacity, NeedCopyData);
  }

  void ReallocateImpl(GLsizeiptr _NewCapacity, bool _NeedCopy)
  {
    m_Capacity = _NewCapacity;

    if (!_NeedCopy)
    {
      glBufferData(m_Target, _NewCapacity, nullptr, m_Usage);
      return;
    }

    GLuint NewBufferID = 0;
    glGenBuffers(1, &NewBufferID);
    glBindBuffer(m_Target, NewBufferID);
    glBufferData(m_Target, _NewCapacity, nullptr, m_Usage);

    glBindBuffer(GL_COPY_READ_BUFFER, m_ID);
    glBindBuffer(GL_COPY_WRITE_BUFFER, NewBufferID);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, m_ActualSize);
    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    glDeleteBuffers(1, &m_ID);
    m_ID = NewBufferID;
  }

  void GenerateBuffer()
  {
    glGenBuffers(1, &m_ID);
  }

  void BindBuffer()
  {
    glBindBuffer(m_Target, m_ID);
  }

  void UnbindBuffer()
  {
    glBindBuffer(m_Target, 0);
  }

  void DeleteBuffer()
  {
    glDeleteBuffers(1, &m_ID);
  }

  GLenum m_Target;
  GLenum m_Usage;
  GLsizeiptr m_Capacity;   // in bytes
  GLsizeiptr m_ActualSize; // in bytes
};

// ------------------------------------------------

class CVertexBuffer final : public CBufferObject
{
public:
  CVertexBuffer(GLenum _Usage) : CBufferObject(GL_ARRAY_BUFFER, _Usage)
  {
  }
};

// ------------------------------------------------

class CElementBuffer final : public CBufferObject
{
public:
  CElementBuffer(GLenum _Usage) : CBufferObject(GL_ELEMENT_ARRAY_BUFFER, _Usage)
  {
  }
};

// ------------------------------------------------

class CUniformBuffer final : public CBufferObject
{
public:
  CUniformBuffer(GLenum _Usage) : CBufferObject(GL_UNIFORM_BUFFER, _Usage)
  {
  }
};

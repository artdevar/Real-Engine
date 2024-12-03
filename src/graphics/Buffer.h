#pragma once

#include <glad/glad.h>
#include <vector>
#include "utils/Common.h"

template <class Derived>
class CBuffer
{
  DISABLE_CLASS_COPY(CBuffer);

public:

  CBuffer(CBuffer && _Other) :
    m_ID(_Other.m_ID)
  {
    _Other.m_ID = 0;
  }

  CBuffer & operator=(CBuffer && _Other)
  {
    if (m_ID != _Other.m_ID)
      Shutdown();

    m_ID = _Other.m_ID;
    _Other.m_ID = 0;

    return *this;
  }

  ~CBuffer()
  {
    Shutdown();
  }

  void Bind()
  {
    assert(m_ID != 0);
    static_cast<Derived*>(this)->BindBuffer();
  }

  void Unbind()
  {
    static_cast<Derived*>(this)->UnbindBuffer();
  }

  GLuint ID() const
  {
    return m_ID;
  }

protected:

  CBuffer() : m_ID(0u)
  {
    Init();
  }

  void Init()
  {
    static_cast<Derived*>(this)->GenerateBuffer();
  }

  void Shutdown()
  {
    if (m_ID)
    {
      Unbind();
      static_cast<Derived*>(this)->DeleteBuffer();
      m_ID = 0;
    }
  }

  GLuint m_ID;
};

// ------------------------------------------------

class CVertexArray final : public CBuffer<CVertexArray>
{
  friend CBuffer;

public:

  void EnableVertexAttrib(GLuint _Index, GLint _Size, GLenum _Type, GLsizei _Stride, const void * _Offset)
  {
    glEnableVertexAttribArray(_Index);
    glVertexAttribPointer(_Index, _Size, _Type, GL_FALSE, _Stride, _Offset);
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
    glBindVertexArray(0);
  }

  void DeleteBuffer()
  {
    glDeleteVertexArrays(1, &m_ID);
  }
};

// ------------------------------------------------

class CBufferObject : public CBuffer<CBufferObject>
{
  friend CBuffer;

public:

  CBufferObject(CBufferObject && _Other) :
    CBuffer<CBufferObject>(std::move(_Other)),
    m_Target(_Other.m_Target),
    m_Usage(_Other.m_Usage),
    m_Capacity(_Other.m_Capacity),
    m_ActualSize(_Other.m_ActualSize)
  {
    _Other.m_Target     = 0;
    _Other.m_Usage      = 0;
    _Other.m_Capacity   = 0;
    _Other.m_ActualSize = 0;
  }

  CBufferObject & operator=(CBufferObject && _Other)
  {
    if (m_ID != _Other.m_ID)
      Shutdown();

    m_ID         = _Other.m_ID;
    m_Target     = _Other.m_Target;
    m_Usage      = _Other.m_Usage;
    m_Capacity   = _Other.m_Capacity;
    m_ActualSize = _Other.m_ActualSize;
    _Other.m_ID         = 0;
    _Other.m_Target     = 0;
    _Other.m_Usage      = 0;
    _Other.m_Capacity   = 0;
    _Other.m_ActualSize = 0;

    return *this;
  }

  template <typename T>
  void Assign(T * _Data, GLuint _Amount)
  {
    const GLsizeiptr DataSizeInBytes  = _Amount * sizeof(T);
    const bool       RegenerateBuffer = m_Capacity != DataSizeInBytes;

    if (RegenerateBuffer)
    {
      glBufferData(m_Target, DataSizeInBytes, nullptr, m_Usage);
      m_Capacity = DataSizeInBytes;
    }

    void * MemPtr = glMapBuffer(m_Target, GL_WRITE_ONLY);
    memcpy(MemPtr, _Data, DataSizeInBytes);
    glUnmapBuffer(m_Target);

    m_ActualSize = DataSizeInBytes;
  }

  template <typename T>
  void Assign(const std::vector<T> & _Data)
  {
    Assign(_Data.data(), _Data.size());
  }

  template <typename T>
  void Push(T * _Data, GLuint _Amount)
  {
    const GLsizeiptr DataSizeInBytes = _Amount * sizeof(T);

    if (m_ActualSize + DataSizeInBytes > m_Capacity)
      Reallocate(m_ActualSize + DataSizeInBytes);

    void * MemPtr = glMapBuffer(m_Target, GL_WRITE_ONLY);
    memcpy(((int8_t*)MemPtr + m_ActualSize), _Data, DataSizeInBytes);
    glUnmapBuffer(m_Target);

    m_ActualSize += DataSizeInBytes;
  }

  template <typename T>
  void Push(const std::vector<T> & _Data)
  {
    Push(_Data.data(), _Data.size());
  }

protected:

  CBufferObject(GLenum _Target, GLenum _Usage) :
    m_Target(_Target),
    m_Usage(_Usage),
    m_Capacity(0),
    m_ActualSize(0)
  {}

  void Reallocate(GLsizeiptr _RequiredSize)
  {
    const bool       NeedCopyData      = m_ActualSize != 0;
    const GLsizeiptr NewBufferCapacity = m_Capacity + static_cast<GLsizeiptr>((_RequiredSize - m_Capacity) * GrowthFactor);

    if (NeedCopyData)
    {
      GLuint NewBufferID = 0;
      glGenBuffers(1, &NewBufferID);
      glBindBuffer(m_Target, NewBufferID);
      glBufferData(m_Target, NewBufferCapacity, nullptr, m_Usage);

      glBindBuffer(GL_COPY_READ_BUFFER,  m_ID);
      glBindBuffer(GL_COPY_WRITE_BUFFER, NewBufferID);
      glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, m_ActualSize);
      glBindBuffer(GL_COPY_READ_BUFFER, 0);
      glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
      glDeleteBuffers(1, &m_ID);
      m_ID = NewBufferID;
    }
    else
    {
      glBufferData(m_Target, NewBufferCapacity, nullptr, m_Usage);
    }

    m_Capacity = NewBufferCapacity;
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

  static constexpr inline float GrowthFactor = 1.3f;

  GLenum     m_Target;
  GLenum     m_Usage;
  GLsizeiptr m_Capacity;   // in bytes
  GLintptr   m_ActualSize; // in bytes
};

// ------------------------------------------------

class CVertexBuffer final : public CBufferObject
{
public:

  CVertexBuffer(GLenum _Usage) : CBufferObject(GL_ARRAY_BUFFER, _Usage)
  {}
};

// ------------------------------------------------

class CElementBuffer final : public CBufferObject
{
public:

  CElementBuffer(GLenum _Usage) : CBufferObject(GL_ELEMENT_ARRAY_BUFFER, _Usage)
  {}
};

// ------------------------------------------------

class CUniformBuffer final : public CBufferObject
{
public:

  CUniformBuffer(GLenum _Usage) : CBufferObject(GL_UNIFORM_BUFFER, _Usage)
  {}
};
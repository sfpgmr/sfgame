#pragma once
namespace sf {
// policy class
struct heap_memory_free_policy
{
template< typename T >
void operator()( const T* AMemory ) const
{
if( NULL != AMemory )
::HeapFree( ::GetProcessHeap(), 0, AMemory );
}
};
// policy class
struct local_memory_free_policy
{
template< typename T >
void operator()( const T* AMemory ) const
{
if( NULL != AMemory )
::LocalFree( AMemory );
}
};
// policy class
struct co_task_memory_free_policy
{
template< typename T >
void operator()( const T* AMemory ) const
{
if( NULL != AMemory )
::CoTaskMemFree( AMemory );
}
};
// base guard class
template< typename T,class TFreePolicy >
class base_memory
{
private:
T *FMemory;

public:
base_memory( T* AMemory = NULL )
: FMemory( AMemory ) {}

virtual ~base_memory( void )
{ reset(); }

T* release( void )
{
T *tmp = FMemory;
FMemory = NULL;
return tmp;
}

void reset( T* AMemory = NULL )
{
if( AMemory != FMemory )
{
if( NULL != FMemory )
TFreePolicy( FMemory );

FMemory = AMemory;
}
}

T* get( void )
{ return FMemory; }

T* operator ->(void)
{
  return FMemory;
}

T** operator&( void )
{ return &FMemory; }

};
template< typename T >
class heap_memory : public base_memory< T, 
heap_memory_free_policy >
{
public:
heap_memory( T* AMemory = NULL )
: base_memory< T, heap_memory_free_policy >( AMemory )
{ }
};
template< typename T >
class local_memory : public base_memory< T, 
local_memory_free_policy >
{
public:
local_memory( T* AMemory = NULL )
: base_memory< T, local_memory_free_policy >( AMemory )
{ }
};
template< typename T >
class co_task_memory : public base_memory< T, co_task_memory_free_policy >
{
public:
co_task_memory( T* AMemory = NULL )
: base_memory< T, co_task_memory_free_policy >( AMemory )
{ }
};

struct handle_deleter {
  typedef HANDLE pointer;
  void operator ()(HANDLE handle) {
      if (handle != INVALID_HANDLE_VALUE) {
          CloseHandle(handle);
      }
  }
};

typedef std::unique_ptr<HANDLE,handle_deleter> handle_holder;

};

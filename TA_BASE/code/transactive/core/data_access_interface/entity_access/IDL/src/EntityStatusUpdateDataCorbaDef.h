// This file is generated by omniidl (C++ backend)- omniORB_4_1. Do not edit.
#ifndef __EntityStatusUpdateDataCorbaDef_hh__
#define __EntityStatusUpdateDataCorbaDef_hh__

#ifndef __CORBA_H_EXTERNAL_GUARD__
#include <omniORB4/CORBA.h>
#endif

#ifndef  USE_stub_in_nt_dll
# define USE_stub_in_nt_dll_NOT_DEFINED_EntityStatusUpdateDataCorbaDef
#endif
#ifndef  USE_core_stub_in_nt_dll
# define USE_core_stub_in_nt_dll_NOT_DEFINED_EntityStatusUpdateDataCorbaDef
#endif
#ifndef  USE_dyn_stub_in_nt_dll
# define USE_dyn_stub_in_nt_dll_NOT_DEFINED_EntityStatusUpdateDataCorbaDef
#endif






#ifdef USE_stub_in_nt_dll
# ifndef USE_core_stub_in_nt_dll
#  define USE_core_stub_in_nt_dll
# endif
# ifndef USE_dyn_stub_in_nt_dll
#  define USE_dyn_stub_in_nt_dll
# endif
#endif

#ifdef _core_attr
# error "A local CPP macro _core_attr has already been defined."
#else
# ifdef  USE_core_stub_in_nt_dll
#  define _core_attr _OMNIORB_NTDLL_IMPORT
# else
#  define _core_attr
# endif
#endif

#ifdef _dyn_attr
# error "A local CPP macro _dyn_attr has already been defined."
#else
# ifdef  USE_dyn_stub_in_nt_dll
#  define _dyn_attr _OMNIORB_NTDLL_IMPORT
# else
#  define _dyn_attr
# endif
#endif





_CORBA_MODULE TA_Base_Core

_CORBA_MODULE_BEG

  struct EntityStatusUpdateCorbaDef {
    typedef _CORBA_ConstrType_Variable_Var<EntityStatusUpdateCorbaDef> _var_type;

    
    ::CORBA::ULong entityKey;

    ::CORBA::String_member lastSessionId;

    ::CORBA::ULong lastModifyDate;

    ::CORBA::String_member entityStatusDataValue;

    ::CORBA::ULong entityStatusType;

    ::CORBA::ULong entityStatusDataType;

  

    void operator>>= (cdrStream &) const;
    void operator<<= (cdrStream &);
  };

  typedef EntityStatusUpdateCorbaDef::_var_type EntityStatusUpdateCorbaDef_var;

  typedef _CORBA_ConstrType_Variable_OUT_arg< EntityStatusUpdateCorbaDef,EntityStatusUpdateCorbaDef_var > EntityStatusUpdateCorbaDef_out;

  _CORBA_MODULE_VAR _dyn_attr const ::CORBA::TypeCode_ptr _tc_EntityStatusUpdateCorbaDef;

  _CORBA_MODULE_VAR _dyn_attr const ::CORBA::TypeCode_ptr _tc_EntityStatusUpdateSequence;

  class EntityStatusUpdateSequence_var;

  class EntityStatusUpdateSequence : public _CORBA_Unbounded_Sequence< EntityStatusUpdateCorbaDef >  {
  public:
    typedef EntityStatusUpdateSequence_var _var_type;
    inline EntityStatusUpdateSequence() {}
    inline EntityStatusUpdateSequence(const EntityStatusUpdateSequence& _s)
      : _CORBA_Unbounded_Sequence< EntityStatusUpdateCorbaDef > (_s) {}

    inline EntityStatusUpdateSequence(_CORBA_ULong _max)
      : _CORBA_Unbounded_Sequence< EntityStatusUpdateCorbaDef > (_max) {}
    inline EntityStatusUpdateSequence(_CORBA_ULong _max, _CORBA_ULong _len, EntityStatusUpdateCorbaDef* _val, _CORBA_Boolean _rel=0)
      : _CORBA_Unbounded_Sequence< EntityStatusUpdateCorbaDef > (_max, _len, _val, _rel) {}

  

    inline EntityStatusUpdateSequence& operator = (const EntityStatusUpdateSequence& _s) {
      _CORBA_Unbounded_Sequence< EntityStatusUpdateCorbaDef > ::operator=(_s);
      return *this;
    }
  };

  class EntityStatusUpdateSequence_out;

  class EntityStatusUpdateSequence_var {
  public:
    inline EntityStatusUpdateSequence_var() : _pd_seq(0) {}
    inline EntityStatusUpdateSequence_var(EntityStatusUpdateSequence* _s) : _pd_seq(_s) {}
    inline EntityStatusUpdateSequence_var(const EntityStatusUpdateSequence_var& _s) {
      if( _s._pd_seq )  _pd_seq = new EntityStatusUpdateSequence(*_s._pd_seq);
      else              _pd_seq = 0;
    }
    inline ~EntityStatusUpdateSequence_var() { if( _pd_seq )  delete _pd_seq; }
      
    inline EntityStatusUpdateSequence_var& operator = (EntityStatusUpdateSequence* _s) {
      if( _pd_seq )  delete _pd_seq;
      _pd_seq = _s;
      return *this;
    }
    inline EntityStatusUpdateSequence_var& operator = (const EntityStatusUpdateSequence_var& _s) {
      if( _s._pd_seq ) {
        if( !_pd_seq )  _pd_seq = new EntityStatusUpdateSequence;
        *_pd_seq = *_s._pd_seq;
      } else if( _pd_seq ) {
        delete _pd_seq;
        _pd_seq = 0;
      }
      return *this;
    }
    inline EntityStatusUpdateCorbaDef& operator [] (_CORBA_ULong _s) {
      return (*_pd_seq)[_s];
    }

  

    inline EntityStatusUpdateSequence* operator -> () { return _pd_seq; }
    inline const EntityStatusUpdateSequence* operator -> () const { return _pd_seq; }
#if defined(__GNUG__)
    inline operator EntityStatusUpdateSequence& () const { return *_pd_seq; }
#else
    inline operator const EntityStatusUpdateSequence& () const { return *_pd_seq; }
    inline operator EntityStatusUpdateSequence& () { return *_pd_seq; }
#endif
      
    inline const EntityStatusUpdateSequence& in() const { return *_pd_seq; }
    inline EntityStatusUpdateSequence&       inout()    { return *_pd_seq; }
    inline EntityStatusUpdateSequence*&      out() {
      if( _pd_seq ) { delete _pd_seq; _pd_seq = 0; }
      return _pd_seq;
    }
    inline EntityStatusUpdateSequence* _retn() { EntityStatusUpdateSequence* tmp = _pd_seq; _pd_seq = 0; return tmp; }
      
    friend class EntityStatusUpdateSequence_out;
    
  private:
    EntityStatusUpdateSequence* _pd_seq;
  };

  class EntityStatusUpdateSequence_out {
  public:
    inline EntityStatusUpdateSequence_out(EntityStatusUpdateSequence*& _s) : _data(_s) { _data = 0; }
    inline EntityStatusUpdateSequence_out(EntityStatusUpdateSequence_var& _s)
      : _data(_s._pd_seq) { _s = (EntityStatusUpdateSequence*) 0; }
    inline EntityStatusUpdateSequence_out(const EntityStatusUpdateSequence_out& _s) : _data(_s._data) {}
    inline EntityStatusUpdateSequence_out& operator = (const EntityStatusUpdateSequence_out& _s) {
      _data = _s._data;
      return *this;
    }
    inline EntityStatusUpdateSequence_out& operator = (EntityStatusUpdateSequence* _s) {
      _data = _s;
      return *this;
    }
    inline operator EntityStatusUpdateSequence*&()  { return _data; }
    inline EntityStatusUpdateSequence*& ptr()       { return _data; }
    inline EntityStatusUpdateSequence* operator->() { return _data; }

    inline EntityStatusUpdateCorbaDef& operator [] (_CORBA_ULong _i) {
      return (*_data)[_i];
    }

  

    EntityStatusUpdateSequence*& _data;

  private:
    EntityStatusUpdateSequence_out();
    EntityStatusUpdateSequence_out& operator=(const EntityStatusUpdateSequence_var&);
  };

_CORBA_MODULE_END



_CORBA_MODULE POA_TA_Base_Core
_CORBA_MODULE_BEG

_CORBA_MODULE_END



_CORBA_MODULE OBV_TA_Base_Core
_CORBA_MODULE_BEG

_CORBA_MODULE_END





#undef _core_attr
#undef _dyn_attr

extern void operator<<=(::CORBA::Any& _a, const TA_Base_Core::EntityStatusUpdateCorbaDef& _s);
extern void operator<<=(::CORBA::Any& _a, TA_Base_Core::EntityStatusUpdateCorbaDef* _sp);
extern _CORBA_Boolean operator>>=(const ::CORBA::Any& _a, TA_Base_Core::EntityStatusUpdateCorbaDef*& _sp);
extern _CORBA_Boolean operator>>=(const ::CORBA::Any& _a, const TA_Base_Core::EntityStatusUpdateCorbaDef*& _sp);

void operator<<=(::CORBA::Any& _a, const TA_Base_Core::EntityStatusUpdateSequence& _s);
void operator<<=(::CORBA::Any& _a, TA_Base_Core::EntityStatusUpdateSequence* _sp);
_CORBA_Boolean operator>>=(const ::CORBA::Any& _a, TA_Base_Core::EntityStatusUpdateSequence*& _sp);
_CORBA_Boolean operator>>=(const ::CORBA::Any& _a, const TA_Base_Core::EntityStatusUpdateSequence*& _sp);





#ifdef   USE_stub_in_nt_dll_NOT_DEFINED_EntityStatusUpdateDataCorbaDef
# undef  USE_stub_in_nt_dll
# undef  USE_stub_in_nt_dll_NOT_DEFINED_EntityStatusUpdateDataCorbaDef
#endif
#ifdef   USE_core_stub_in_nt_dll_NOT_DEFINED_EntityStatusUpdateDataCorbaDef
# undef  USE_core_stub_in_nt_dll
# undef  USE_core_stub_in_nt_dll_NOT_DEFINED_EntityStatusUpdateDataCorbaDef
#endif
#ifdef   USE_dyn_stub_in_nt_dll_NOT_DEFINED_EntityStatusUpdateDataCorbaDef
# undef  USE_dyn_stub_in_nt_dll
# undef  USE_dyn_stub_in_nt_dll_NOT_DEFINED_EntityStatusUpdateDataCorbaDef
#endif

#endif  // __EntityStatusUpdateDataCorbaDef_hh__

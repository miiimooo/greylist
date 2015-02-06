// Solution 4 (final solution)
// note: compiles with gcc 3.2, VC6 and
// Comeau++ (http://www.comeaucomputing.com/tryitout/)
#include "message_handler_log.h"
#include <string>
#include <queue>
#include <vector>
#include <map>
#include <assert.h>
#include <algorithm>
// comment this line and uncomment the following one,
// in order to use BOOST
#ifdef _WIN32
#define USE_WIN32_THREAD_MANAGER
#else
#define USE_BOOST_THREAD_MANAGER
#endif
////////////////////////////////////////////////////////////////////
// thread managers
#ifdef USE_WIN32_THREAD_MANAGER
#include <windows.h>
// the object to be started - on a given thread
struct win32_thread_obj
{
  virtual ~win32_thread_obj() {}
  virtual void operator()() = 0;
};
struct win32_thread_manager
{
  typedef win32_thread_obj thread_obj_base;
  static void sleep(int nMillisecs) {
    Sleep(nMillisecs);
  }
  static void create_thread(win32_thread_obj & obj)
  {
    DWORD dwThreadID;
    CreateThread(0, 0,
                 win32_thread_manager::ThreadProc, &obj, 0, &dwThreadID);
  }
  // critical section for Win32
  class critical_section
  {
      critical_section & operator = (const critical_section & Not_Implemented);
      critical_section(const critical_section & From);
    public:
      critical_section() {
        InitializeCriticalSection(GetCsPtr());
      }
      ~critical_section() {
        DeleteCriticalSection(GetCsPtr());
      }
      void Lock() {
        EnterCriticalSection(GetCsPtr());
      }
      void Unlock() {
        LeaveCriticalSection(GetCsPtr());
      }
      operator LPCRITICAL_SECTION() const {
        return GetCsPtr();
      }
    private:
      LPCRITICAL_SECTION GetCsPtr() const {
        return &m_cs;
      }
    private:
      // the critical section itself
      mutable CRITICAL_SECTION m_cs;
  };
  // automatic locking/unlocking of a resource
  class auto_lock_unlock
  {
      auto_lock_unlock operator=(auto_lock_unlock & Not_Implemented);
      auto_lock_unlock(const auto_lock_unlock & Not_Implemented);
    public:
      auto_lock_unlock(critical_section & cs) : m_cs(cs) {
        m_cs.Lock();
      }
      ~auto_lock_unlock() {
        m_cs.Unlock();
      }
    private:
      critical_section & m_cs;
  };
private:
  static DWORD WINAPI ThreadProc(LPVOID lpData)
  {
    win32_thread_obj * pThread = (win32_thread_obj *)lpData;
    (*pThread)();
    return 0;
  }
};
#endif // ifdef USE_WIN32_THREAD_MANAGER
#ifdef USE_BOOST_THREAD_MANAGER
#include "boost/thread/mutex.hpp"
#include "boost/thread/thread.hpp"
#include "boost/function.hpp"
#include "boost/thread/xtime.hpp"
class boost_thread_manager
{
  public:
    struct thread_obj_base
    {
      virtual void operator()() = 0;
    };
  private:
    struct function_wrapper
    {
      function_wrapper(thread_obj_base & base) : m_base(base) {}
      void operator()() const {
        m_base();
      }
    private:
      mutable thread_obj_base & m_base;
    };
  public:
    static void sleep(int nMillisecs)
    {
      boost::xtime xt;
      boost::xtime_get(&xt, boost::TIME_UTC);
      // Sleep for n Millisecs
      xt.nsec += 1000000 * nMillisecs
                 + 100000 /* just in case*/;
      boost::thread::sleep(xt);
    }
    static void create_thread(thread_obj_base & obj)
    {
      boost::function0< void> f = function_wrapper(obj);
      // creates the thread
      boost::thread t(f);
    }
    typedef boost::mutex critical_section;
    typedef boost::mutex::scoped_lock auto_lock_unlock;
};
#endif // #ifdef USE_BOOST_THREAD_MANAGER
// if you want a custom default manager,
// create your custom thread_manager class, like
// the ones shown in win32_thread_manager or
// boost_thread_manager, and
// #define DEFAULT_THREAD_MANAGER <your_custom_class>
//
#if defined( DEFAULT_THREAD_MANAGER)
// custom thread manager
#elif defined( USE_WIN32_THREAD_MANAGER)
#define DEFAULT_THREAD_MANAGER win32_thread_manager
#elif defined( USE_BOOST_THREAD_MANAGER)
#define DEFAULT_THREAD_MANAGER boost_thread_manager
#else
#error "No thread manager. #define either of USE_WIN32_THREAD_MANAGER, USE_BOOST_THREAD_MANAGER, DEFAULT_THREAD_MANAGER"
#endif
// END OF thread managers
////////////////////////////////////////////////////////////////////
// forward declaration
template < class char_type, class traits_type = std::char_traits< char_type> >
class basic_thread_safe_log;
// base class for our internal thread_safe_log object
template< class char_type, class traits_type>
class basic_internal_thread_safe_log_base
{
    typedef basic_internal_thread_safe_log_base< char_type, traits_type> this_class;
    typedef typename std::basic_ostream< char_type, traits_type> ostream_type;
    friend class basic_thread_safe_log< char_type, traits_type>;
    // non-copyiable
    basic_internal_thread_safe_log_base(const this_class &);
    this_class & operator=(this_class &);
  public:
    virtual void write_message(const std::basic_string< char_type, traits_type> & str) = 0;
    virtual void copy_state_to(ostream_type & dest) const = 0;
    virtual void copy_state_from(const ostream_type & src) = 0;
  protected:
    basic_internal_thread_safe_log_base() {}
}; // class basic_internal_thread_safe_log_base
////////////////////////////////////////////////////////////////////
// internal_thread_safe_log for SharedThread
// (multiple logs share the same thread for writing to them)
// forward declaration
template <
class char_type,
class traits_type = std::char_traits< char_type>,
class thread_manager = DEFAULT_THREAD_MANAGER >
class basic_internal_thread_safe_log_sharethread;
// allows thread-safe writing for multiple logs
template <
class char_type,
class traits_type = std::char_traits< char_type>,
class thread_manager = DEFAULT_THREAD_MANAGER >
class basic_thread_safe_log_writer_sharethread
{
    typedef basic_thread_safe_log_writer_sharethread< char_type, traits_type> this_class;
    typedef std::basic_ostream< char_type, traits_type> ostream_type;
    typedef std::basic_string< char_type, traits_type> string_type;
    friend class basic_internal_thread_safe_log_sharethread< char_type, traits_type, thread_manager>;
    // copying not allowed
    basic_thread_safe_log_writer_sharethread(const this_class &);
    this_class & operator=(const this_class &);
    // forward declaration
    struct thread_info;
    friend struct thread_info;
    // thread-related definitions
    typedef typename thread_manager::thread_obj_base thread_obj_base;
    typedef typename thread_manager::critical_section critical_section;
    typedef typename thread_manager::auto_lock_unlock auto_lock_unlock;
    // so that from our thread we know the object we're manipulating
    struct thread_info : public thread_obj_base
    {
      thread_info()
          : m_bHasFinished(false),
          m_pThis(NULL)
      {}
      /* virtual */ void operator()()
      {
        while (true)
        {
          // ... we might be writing multiple messages at once!
          std::vector< std::string *> astrMsgs;
          ostream_type * pLog = NULL;
          bool bDoFlush = false;
          {
            auto_lock_unlock locker(m_pThis->m_cs);
            if (m_pThis->m_nSumOfPriorities <= 0)
            {
              // we don't have any logs yet...
              thread_manager::sleep(1);
              continue;
            }
            // find a log that has messages to be written to it
            for (int idx = 0; idx < m_pThis->m_nSumOfPriorities; ++idx)
            {
              LogWrites & writes = *(m_pThis->m_aWritesTo[ m_pThis->m_idxWrite]);
              if (writes.m_astr.size() > 0)
                // we found a log that we should write to
                break;
              ++m_pThis->m_idxWrite;
              m_pThis->m_idxWrite %= m_pThis->m_nSumOfPriorities;
            }
            // did we find a log with messages that should be written to it?
            LogWrites & writes = *(m_pThis->m_aWritesTo[ m_pThis->m_idxWrite]);
            if (writes.m_astr.size() > 0)
            {
              // we get the string(s) to write to this log
              pLog = writes.m_pDestLog;
              // optimization - if too many messages, write
              // multiple messages at once
              int nMessages = 1;
              if (writes.m_astr.size() > 100)
                nMessages = writes.m_astr.size() / 10;
              while (nMessages > 0)
              {
                astrMsgs.push_back(writes.m_astr.front());
                writes.m_astr.pop();
                --nMessages;
              }
              // we flush only when there are no more messages to write
              // (flushing could be time-consuming)
              bDoFlush = (writes.m_astr.size() == 0);
            }
            // ... only when there are no more messages,
            //    will we ask if we should be destructed
            else if (m_pThis->m_bShouldBeDestructed)
            {
              // signal to the other thread we've finished
              m_bHasFinished = true;
              return;
            }
          }
          // write the string(s)
          if (astrMsgs.size() > 0)
          {
            std::vector< std::string *>::iterator
            first = astrMsgs.begin(), last = astrMsgs.end();
            while (first != last)
            {
              std::string *pstr = *first;
              *pLog << *pstr;
              delete pstr;
              ++first;
            }
            if (bDoFlush)
              pLog->flush();
          }
          else
            // nothing to write - wait
            thread_manager::sleep(1);
        }
      }
      this_class * m_pThis;
      volatile bool m_bHasFinished;
    };
  public:
    basic_thread_safe_log_writer_sharethread()
        : m_bShouldBeDestructed(false)
    {
      m_info.m_pThis = this;
      thread_manager::create_thread(m_info);
      m_nSumOfPriorities = 0;
      m_idxWrite = 0;
    }
    ~basic_thread_safe_log_writer_sharethread()
    {
      // signal to the other thread we're about to be
      // destructed
      {
        auto_lock_unlock locker(m_cs);
        m_bShouldBeDestructed = true;
      }
      // wait while the other thread writes all messages
      while (true)
      {
        auto_lock_unlock locker(m_cs);
        if (m_info.m_bHasFinished)
          // the other thread has finished
          break;
      }
    }
  private:
    // note: only basic_internal_thread_safe_log can
    // call these functions
    // adds a message to be written to a given log
    void add_message(const string_type & str, ostream_type & log)
    {
      auto_lock_unlock locker(m_cs);
      ostream_type * pLog = &log;
      m_collLogWrites[ pLog].m_astr.push(new string_type(str));
    }
    // adds a log we can write to, with a given priority
    void add_log(ostream_type & log, int nPriority)
    {
      // priority should be at least one
      assert(nPriority > 0);
      auto_lock_unlock locker(m_cs);
      ostream_type * pLog = &log;
      m_collLogWrites[ pLog].m_nLogPriority = nPriority;
      m_collLogWrites[ pLog].m_pDestLog = pLog;
      m_nSumOfPriorities += nPriority;
      m_idxWrite = 0;
      m_aWritesTo.resize(m_nSumOfPriorities);
      std::fill(m_aWritesTo.begin(), m_aWritesTo.end(), (LogWrites *)0);
      typename LogWritesCollection::iterator
      first = m_collLogWrites.begin(), last = m_collLogWrites.end();
      while (first != last)
      {
        LogWrites & writes = first->second;
        for (int idx = 0; idx < writes.m_nLogPriority; ++idx)
        {
          int idxWrite =
            (int)((double)(idx * m_nSumOfPriorities) / writes.m_nLogPriority);
          // ... find an empty spot
          while (m_aWritesTo[ idxWrite] != 0)
          {
            ++idxWrite;
            idxWrite = idxWrite % m_nSumOfPriorities;
          }
          m_aWritesTo[ idxWrite] = &writes;
        }
        ++first;
      }
    }
    critical_section & cs() const {
      return m_cs;
    }
  private:
    // the critical section used for thread-safe locking
    mutable critical_section m_cs;
    // needed to create the other thread
    thread_info m_info;
    volatile bool m_bShouldBeDestructed;
    typedef std::queue< string_type* > StringsQueue;
    // forward declaration;
    struct LogWrites;
    friend struct LogWrites;
    struct LogWrites
    {
      LogWrites()
          : m_nLogPriority(0), m_pDestLog(NULL) {}
      // the priority of this log
      int m_nLogPriority;
      // the strings to write to this log
      StringsQueue m_astr;
      // the log we should write to
      ostream_type * m_pDestLog;
    };
    // at each step, from which log should we write to?
    std::vector< LogWrites* > m_aWritesTo;
    // for each log, what should we write to it?
    typedef std::map< ostream_type*, LogWrites> LogWritesCollection;
    LogWritesCollection m_collLogWrites;
    // the sum of all log' priorities
    int m_nSumOfPriorities;
    // the index of the current write
    // ( always less than m_nSumOfPriorities)
    int m_idxWrite;
};
typedef basic_thread_safe_log_writer_sharethread< char> thread_safe_log_writer_sharethread;
typedef basic_thread_safe_log_writer_sharethread< wchar_t> wthread_safe_log_writer_sharethread;
// multiple basic_internal_thread_safe logs share the same thread,
// which writes to them
template< class char_type, class traits_type, class thread_manager>
class basic_internal_thread_safe_log_sharethread
      : public basic_internal_thread_safe_log_base< char_type, traits_type>
{
    typedef basic_internal_thread_safe_log_sharethread< char_type, traits_type, thread_manager> this_class;
    typedef typename std::basic_ostream< char_type, traits_type> ostream_type;
    typedef class basic_thread_safe_log_writer_sharethread< char_type, traits_type, thread_manager> multiple_log_writer;
    // non-copyiable
    basic_internal_thread_safe_log_sharethread(const this_class &);
    this_class & operator=(this_class &);
    // thread-related definitions
    typedef typename thread_manager::auto_lock_unlock auto_lock_unlock;
  public:
    basic_internal_thread_safe_log_sharethread(
      ostream_type & underlyingLog,
      multiple_log_writer & writer, int nPriority)
        : m_underlyingLog(underlyingLog),
        m_writer(writer)
    {
      writer.add_log(m_underlyingLog, nPriority);
    }
    ~basic_internal_thread_safe_log_sharethread()
    {}
    void write_message(const std::basic_string< char_type, traits_type> & str)
    {
      m_writer.add_message(str, m_underlyingLog);
    }
    void copy_state_to(ostream_type & dest) const
    {
      auto_lock_unlock locker(m_writer.cs());
      dest.copyfmt(m_underlyingLog);
      dest.setstate(m_underlyingLog.rdstate());
    }
    void copy_state_from(const ostream_type & src)
    {
      auto_lock_unlock locker(m_writer.cs());
      m_underlyingLog.copyfmt(src);
      m_underlyingLog.setstate(m_underlyingLog.rdstate());
    }
  private:
    ostream_type & m_underlyingLog;
    // IMPORTANT: keep it by reference!
    multiple_log_writer & m_writer;
};
typedef basic_internal_thread_safe_log_sharethread< char> internal_thread_safe_log_sharethread;
typedef basic_internal_thread_safe_log_sharethread< wchar_t> winternal_thread_safe_log_sharethread;
// END OF internal_thread_safe_log for SharedThread
// (multiple logs share the same thread for writing to them)
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// internal_thread_safe_log for OwnThread
// (each log has its own thread for writing messages to it)
// allows thread-safe writing
template <
class char_type,
class traits_type = std::char_traits< char_type>,
class thread_manager = DEFAULT_THREAD_MANAGER >
class thread_safe_log_writer_ownthread
{
    typedef thread_safe_log_writer_ownthread< char_type, traits_type> this_class;
    typedef std::basic_ostream< char_type, traits_type> ostream_type;
    typedef std::basic_string< char_type, traits_type> string_type;
    // forward declaration
    struct thread_info;
    friend struct thread_info;
    // thread-related definitions
    typedef typename thread_manager::thread_obj_base thread_obj_base;
    typedef typename thread_manager::critical_section critical_section;
    typedef typename thread_manager::auto_lock_unlock auto_lock_unlock;
    // so that from our thread we know the object we're manipulating
    struct thread_info : public thread_obj_base
    {
      thread_info()
          : m_bHasFinished(false),
          m_pThis(NULL)
      {}
      /* virtual */ void operator()()
      {
        while (true)
        {
          std::string * pstr = NULL;
          bool bDoFlush = false;
          {
            auto_lock_unlock locker(m_pThis->m_cs);
            // get the string
            if (m_pThis->m_astrMessages.size() > 0)
            {
              pstr = m_pThis->m_astrMessages.front();
              m_pThis->m_astrMessages.pop();
              // we flush only when there are no more messages to write
              // (flushing could be time-consuming)
              bDoFlush = (m_pThis->m_astrMessages.size() == 0);
            }
            // ... only when there are no more messages,
            //    will we ask if we should be destructed
            else if (m_pThis->m_bShouldBeDestructed)
            {
              // signal to the other thread we've finished
              m_bHasFinished = true;
              return;
            }
          }
          // write the string
          if (pstr)
          {
            m_pThis->m_underlyingLog << *pstr;
            if (bDoFlush)
              m_pThis->m_underlyingLog.flush();
            delete pstr;
          }
          else
            // nothing to write - wait
            thread_manager::sleep(1);
        }
      }
      this_class * m_pThis;
      volatile bool m_bHasFinished;
    };
  public:
    void add_message(const string_type & str)
    {
      auto_lock_unlock locker(m_cs);
      m_astrMessages.push(new string_type(str));
    }
    thread_safe_log_writer_ownthread(ostream_type & underlyingLog)
        : m_underlyingLog(underlyingLog),
        m_bShouldBeDestructed(false)
    {
      m_info.m_pThis = this;
      thread_manager::create_thread(m_info);
    }
    ~thread_safe_log_writer_ownthread()
    {
      // signal to the other thread we're about to be
      // destructed
      {
        auto_lock_unlock locker(m_cs);
        m_bShouldBeDestructed = true;
      }
      // wait while the other thread writes all messages
      while (true)
      {
        auto_lock_unlock locker(m_cs);
        if (m_info.m_bHasFinished)
          // the other thread has finished
          break;
      }
    }
    critical_section & cs() const {
      return m_cs;
    }
  private:
    // the critical section used for thread-safe locking
    mutable critical_section m_cs;
    // needed to create the other thread
    thread_info m_info;
    volatile bool m_bShouldBeDestructed;
    ostream_type & m_underlyingLog;
    std::queue< string_type*> m_astrMessages;
};
template <
class char_type,
class traits_type = std::char_traits< char_type>,
class thread_manager = DEFAULT_THREAD_MANAGER >
class basic_internal_thread_safe_log_ownthread
      : public basic_internal_thread_safe_log_base< char_type, traits_type>
{
    typedef std::basic_ostream< char_type, traits_type> ostream_type;
    friend class basic_thread_safe_log< char_type, traits_type>;
    typedef thread_safe_log_writer_ownthread< char_type, traits_type, thread_manager> log_writer_ownthread;
    // non-copyiable
    typedef basic_internal_thread_safe_log_ownthread< char_type, traits_type> this_class;
    basic_internal_thread_safe_log_ownthread(const this_class &);
    this_class & operator=(this_class &);
    // thread-related definitions
    typedef typename thread_manager::auto_lock_unlock auto_lock_unlock;
  public:
    basic_internal_thread_safe_log_ownthread(ostream_type & underlyingLog)
        : m_underlyingLog(underlyingLog),
        m_writer(underlyingLog)
    {}
    ~basic_internal_thread_safe_log_ownthread()
    {}
    void write_message(const std::basic_string< char_type, traits_type> & str)
    {
      m_writer.add_message(str);
    }
    void copy_state_to(ostream_type & dest) const
    {
      auto_lock_unlock locker(m_writer.cs());
      dest.copyfmt(m_underlyingLog);
      dest.setstate(m_underlyingLog.rdstate());
    }
    void copy_state_from(const ostream_type & src)
    {
      auto_lock_unlock locker(m_writer.cs());
      m_underlyingLog.copyfmt(src);
      m_underlyingLog.setstate(m_underlyingLog.rdstate());
    }
  private:
    ostream_type & m_underlyingLog;
    log_writer_ownthread m_writer;
};
typedef basic_internal_thread_safe_log_ownthread< char> internal_thread_safe_log_ownthread;
typedef basic_internal_thread_safe_log_ownthread< wchar_t> winternal_thread_safe_log_ownthread;
// END OF internal_thread_safe_log for OwnThread
// (each log has its own thread for writing messages to it)
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// thread_safe_log class;
// the class we should return from our get_log() functions.
// helper used on debug mode - to allow catching mistakes:
// using temporaries after they've been destructed
template< class char_type, class traits_type>
class invalid_streambuf : public std::basic_streambuf< char_type, traits_type>
{
#ifndef __GNUC__
    typedef std::basic_streambuf< char_type, traits_type> base_class;
    using typename base_class::int_type;
#endif
  protected:
    void bad_call()
    {
      /*
       Mistake: most likely, you've done something like:
       std::ostream & out = get_log().ts();
       // note: here, the temporary returned by get_log().ts() has
       //      been destructed !!!
       out << "bla bla" << std::endl;
       Use the following instead:
       thread_safe_log out = get_log();
       out.ts() << "bla bla" << std::endl;
       */
      assert(false);
    }
    // ... called on flush()
    virtual int sync()
    {
      bad_call(); return 0;
    }
    virtual typename std::basic_streambuf< char_type, traits_type>::int_type overflow(typename std::basic_streambuf< char_type, traits_type>::int_type nChar)
    {
      bad_call(); return 0;
    }
    virtual std::streamsize xsputn(const char_type *S, std::streamsize N)
    {
      bad_call(); return 0;
    }
  public:
    static invalid_streambuf< char_type, traits_type> s_instance;
};
// initialize the one and only instance
template< class char_type, class traits_type>
invalid_streambuf< char_type, traits_type> invalid_streambuf< char_type, traits_type>::s_instance;
template< class char_type, class traits_type>
class basic_thread_safe_log
      // *** protected, not public !!!
      : protected basic_message_handler_log< char_type, traits_type>
{
    typedef std::basic_ostream< char_type, traits_type> ostream_type;
    // hold reference to base basic_internal_thread_safe_log;
    // it could be basic_internal_thread_safe_log_sharethread,
    // basic_internal_thread_safe_log_ownthread, etc.
    typedef basic_internal_thread_safe_log_base< char_type, traits_type> internal_type;
//     typedef typename char_type::string_type string_type;
#ifndef __GNUC__
    typedef basic_message_handler_log< char_type, traits_type> base_class;
    using typename base_class::string_type;
#endif
//     typedef basic_message_handler_log< char_type, traits_type> base_class;
    typedef typename basic_message_handler_log< char_type, traits_type>::string_type string_type;
  public:
    basic_thread_safe_log(internal_type & tsLog)
        : m_tsLog(tsLog)
    {
      // get underlying stream state
      tsLog.copy_state_to(ts());
    }
    basic_thread_safe_log(const basic_thread_safe_log< char_type, traits_type> & from)
        : m_tsLog(from.m_tsLog),
        // ... on some platforms, a std::ostream base copy-constructor
        //    might be defined as private...
        basic_message_handler_log< char_type, traits_type>()
    {
      // get underlying stream state
      m_tsLog.copy_state_to(ts());
    }
    ~basic_thread_safe_log()
    {
      // copy state to underlying stream
      m_tsLog.copy_state_from(ts());
      //TODO mimo
      //       this->write_last_message(*this);
#ifndef NDEBUG
      // debug-mode
      this->rdbuf(&(invalid_streambuf< char_type, traits_type>::s_instance));
#endif
    }
    // get base class - to which we can write
    std::basic_ostream< char_type, traits_type> & ts()
    {
      return *this;
    }
    void on_last_message(const string_type & str)
    {
      // don't forget to flush the stream before it's destructed!!!
      // (the easiest way is to append 'std::endl' to it
      assert(str.empty());
    }
  protected:
    virtual void on_new_message(const string_type & str)
    {
      if (str.empty())
        return;
      m_tsLog.write_message(str);
    }
  private:
    internal_type  & m_tsLog;
};
typedef basic_thread_safe_log< char> thread_safe_log;
typedef basic_thread_safe_log< wchar_t> wthread_safe_log;
// END OF thread_safe_log class;
// the class we should return from our get_log() functions.
////////////////////////////////////////////////////////////////////
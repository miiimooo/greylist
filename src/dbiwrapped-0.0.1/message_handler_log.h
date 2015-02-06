// message_handler_log.h
 #ifndef MESSAGE_HANDLER_LOG_H
 #define MESSAGE_HANDLER_LOG_H
 #include <ostream>
 #include <sstream>
 #include <memory>
 #include <assert.h>
 // forward declaration(s)
 template< class char_type, class traits_type = std::char_traits< char_type> >
    class basic_message_handler_log;
 //    represents the stream buffer for a message_handler_log (see below)
 //    Note: NOT thread-safe
 template< class char_type , class traits_type = std::char_traits< char_type> >
    class basic_message_handler_log_streambuf
        : public std::basic_streambuf< char_type, traits_type>
 {
 private:
    friend class basic_message_handler_log< char_type, traits_type>;
    typedef std::basic_streambuf< char_type, traits_type> streambuf_type;
    typedef basic_message_handler_log< char_type, traits_type> ostream_type;
    typedef typename traits_type::int_type int_type;
    typedef typename traits_type::pos_type pos_type;
    typedef typename traits_type::off_type off_type;
    // base class
    typedef std::basic_streambuf< char_type, traits_type> base_class;
 #ifndef __GNUC__
    using typename base_class::int_type;
    using typename base_class::pos_type;
    using typename base_class::off_type;
 #endif
    enum
    { _BADOFF = -1 /* bad offset - for positioning functions */ };
 protected:
    // input, not allowed
    virtual int_type pbackfail(int_type = traits_type::eof())
    {
        // only for output, not for input
        assert( false);
        return (traits_type::eof());
    }
    virtual std::streamsize showmanyc()
    {
        // only for output, not for input
        assert( false);
        return 0;
    }
    virtual int_type underflow()
    {
        // only for output, not for input
        assert( false);
        return (traits_type::eof());
    }
    virtual int_type uflow()
    {
        // only for output, not for input
        assert( false);
        return (traits_type::eof());
    }
    virtual std::streamsize xsgetn(char_type *, std::streamsize)
    {
        // only for output, not for input
        assert( false);
        return 0;
    }
    // positioning, not allowed - we're a log
    virtual pos_type seekoff(off_type, std::ios_base::seekdir,
        std::ios_base::openmode = std::ios_base::in | std::ios_base::out)
    {
        // we don't allow positioning
        assert( false);
        return (std::streampos( _BADOFF));
    }
    virtual pos_type seekpos(pos_type,
        std::ios_base::openmode = std::ios_base::in | std::ios_base::out)
    {
        // we don't allow positioning
        assert( false);
        return (std::streampos( _BADOFF));
    }
    // output functions
    // called to write out from the internal
    // buffer, into the external buffer
    virtual int sync()
    {
       m_pOwnerStream->on_new_message( get_stream_buffer().str() );
        m_pStreamBuffer = std::auto_ptr< string_stream_type>( new string_stream_type);
        return 0;
    }
    virtual streambuf_type *setbuf( char_type * buffer, std::streamsize n)
    {
        // ... note: this function MUST be called
        //    before working with this stream buffer
        // we don't use a buffer - we forward everything
        assert( buffer == NULL && n == 0);
        this->setp( NULL, NULL);
        return this;
    }
    // write the characters from the buffer
    // to their real destination
    virtual int_type overflow(int_type nChar = traits_type::eof())
    {
        if ( traits_type::not_eof( nChar))
            get_stream_buffer() << ( char_type)nChar;
        return traits_type::not_eof( nChar);
    }   
    virtual std::streamsize xsputn(const char_type *S, std::streamsize N)
    {
        get_stream_buffer().write( S, N);
        return N;
    }
 public:
    basic_message_handler_log_streambuf()
        : m_pStreamBuffer( new string_stream_type) {}
 private:
    typedef std::basic_ostringstream< char_type> string_stream_type;
    string_stream_type & get_stream_buffer()
    { return *m_pStreamBuffer; }
 private:
    // holds the Message, until it's flushed
    std::auto_ptr< string_stream_type> m_pStreamBuffer;
    // the Message Handler Log - where we write into
    ostream_type * m_pOwnerStream;
 };
 // derive your class from this and implement the PROTECTED on_new_message function
 template< class char_type, class traits_type >
    class basic_message_handler_log
        : public std::basic_ostream< char_type, traits_type>
 {
    typedef basic_message_handler_log_streambuf< char_type, traits_type> handler_streambuf_type;
    friend class basic_message_handler_log_streambuf< char_type, traits_type>;
    typedef std::basic_ostream< char_type, traits_type> base_class;
    typedef std::basic_ostringstream< char_type> string_stream_type;
 protected:
    typedef std::basic_string< char_type> string_type;
    basic_message_handler_log()
       : m_StreamBuf(),
          base_class( NULL)
    {
        m_StreamBuf.m_pOwnerStream = this;
        this->init( &m_StreamBuf);
        m_StreamBuf.pubsetbuf( NULL, 0);
    }
    basic_message_handler_log( const basic_message_handler_log< char_type, traits_type> & from)
        : m_StreamBuf(),
          base_class( NULL)
    {
        m_StreamBuf.m_pOwnerStream = this;
        this->init( &m_StreamBuf);
        m_StreamBuf.pubsetbuf( NULL, 0);
    }
    virtual ~basic_message_handler_log() {}
 protected:
    virtual void on_new_message( const string_type & new_message_log) = 0;
 public:
    // our stream buffer
    handler_streambuf_type m_StreamBuf;
 };
 typedef basic_message_handler_log< char> message_handler_log;
 typedef basic_message_handler_log< wchar_t> wmessage_handler_log;
 #endif

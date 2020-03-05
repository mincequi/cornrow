Landing page for http::Server {#landingpage}
============

This specification outlines the API to add a custom landing page to the
zeroeq::http::Server. Currently
[Tide](https://github.com/BlueBrain/Tide/blob/master/tide/master/rest/RestInterface.cpp)
uses a hack to achieve static html pages.

## Requirements

* Custom, runtime-changeable index.html landing page
* Default landing page displays a pretty-formatted version of /registry
* Allow dynamic pages

## Dependencies

* [154](https://github.com/HBPVIS/ZeroEQ/pull/154): Vocabulary API

## API

    namespace http
    {
    class Page
    {
         virtual std::string getContent() = 0;
    };

    class Server
    {
        void setIndex( Page& page );
    };
    }

## Protocol

* "GET / HTTP/1.0" and "GET /index.html HTTP/1.0" deliver the same
  landing page

## Implementation

* New 'index' member in Server::Impl
* Extend _processGet to handle empty and index.html URLs
* Serves current Page::getContent on each invocation
* Default index page generates page from registry

## Issues

### Issue 1: Should we handle more than one page?

Resolution: Not for now.

There is a risk of feature creep, but likely these extra developments
will be needed at some point. For instance, Tide could benefit from a
more advanced web UI. It would complement the touch interface and
replace the complicated master application & VNC setup. This implies
delivering images and javascript in addition to html. In the absence of
a proper API, this will happen through more "hacking" of ```class
StaticContent : public servus::Serializable```, or switching to a
different network library.

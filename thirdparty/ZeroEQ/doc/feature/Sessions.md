Sessions filtering and management port {#Sessions}
============

# Motivation

Before this document, only publishers were discoverable via ZeroConf, and the
announced information was weak for filtering for supported events and any other
semantics. Also, subscribers always subscribed automatically to every discovered
publisher which used the same schema.

To overcome already applied workarounds and missing features for filtering,
application-compatible subscriptions and user-controllable subscriptions, this
document describes necessary changes to ZeroEQ.

Use-cases are coupling of applications that belong to a certain user/session,
not coupling all applications on the same network. User-interfaces for selection
of applications that should connect to each other need more information for the
user and for the system to perform filtering.

# Overview of changes

* Introduction of Session: only publishers of the same session and compatible
  events are automatically subscribed in the subscriber. The default session
  name is the current username. It can be set to a different value at runtime
  with the ZEROEQ_PUB_SESSION environment variable. If the session name is
  empty/non-existant, no filtering is performed and automatic subscription to
  all discovered publishers happens.
* Schemas in URIs are superseded by the session; user-specified schemas are
  ignored. Filtering is now done via sessions and compatible events. As a future
  use-case, the schema might control the transport for zmq (tcp, inproc, ...)
* The ZeroConf record contains information about published/subscribed events,
  application name (derived automatically), username and session.
* Vocabulary stores event type information from an associated publisher or
  subscriber. The vocabulary information can be requested (this class implements
  request-reply using the REQ-REP socket from zmq). One vocabulary is associated
  with exactly one publisher or with one subscriber.
* Broker can handle explicit (un)subscriptions instructed by a remote
  application.

# Changes for Publisher

* ZeroConf/Servus service name is _zeroeq_pub._tcp
* Schema is fixed, internal and ignored if specified
* URI can contain hostname and port
* ZeroConf announcement:
  * Instance name is hostname:port
  * txt record:
    * vocabulary_request = zmq connection string
    * vocabulary = semi-colon separated list of published event UUIDs
    * session = <user-name> by default, env ZEROEQ_PUB_SESSION if specified
    * user = <user-name>
    * application = <application-name>
* Published events must be registered in the publisher. Non-registered events
  cannot be published. (TBD: Published events are auto-registered).
* Add (un)registerEvent()

# Changes for Subscriber

* ZeroConf/Servus service name is _zeroeq_sub._tcp
* Schema is fixed, internal and ignored if specified
* URI can contain hostname and port
* Automatic subscription only via session (default session is username),
  otherwise explicit subscription via Broker.
* ZeroConf announcement:
  * Instance name is a UUID to uniquely identify this subscriber
  * txt record:
    * vocabulary_request = zmq connection string
    * vocabulary = semi-colon separated list of published event UUIDs
    * session = <user-name> by default, env ZEROEQ_PUB_SESSION if specified
    * user = <user-name>
    * application = <application-name>
* Event registration for vocabulary happens in
  registerHandler/subscribe

# Changes for Broker/Management port

* Supports subscribe/unsubscribe requests for any remote application
  * Tell application to subscribe to an event of a publisher
* List of current subscriptions (for NodeGraph UI for instance)

# New class Vocabulary

* Provides a request-reply for vocabulary information
* Holds registered events from a publisher or subscriber

Publishing the REST API exposed by the HTTP Server {#restapi}
============

The http server exposes C++ objects via a REST API, in a JSON serialized format.
The purpose of this document is to describe how this API request is implemented
and invoked.

## Requirements

* Define how to invoke the application REST API
* Implement the JSON representation of the REST API

## API

* HTTP
  * _GET /registry_ returns the URIs for all registered objects, together with
    the supported REST operation (GET or PUT).
  * Each object can be queried for its schema using a GET operation on the URI
    of that object, suffixed by /schema. The format of the schema is opaque to
    the ZeroEQ http server, but using JSON schema (http://json-schema.org/) to
    describe the JSON data provided by servus::Serializable is recommended.
    Objects which did not implement servus::Serializable::getSchema() do not
    provide the /schema endpoint and hence return an error 404.
* C++
  * zeroeq::http::Server
    * New: `bool handlePUT( const std::string& event, const std::string& schema,
                            const PUTFunc& func );`
    * New: `bool handlePUT( const std::string& event, const std::string& schema,
                            const PUTPayloadFunc& func );`
    * New: `bool handleGET( const std::string& event, const std::string& schema,
                            const GETFunc& func );`
  * servus::Serializable
    * New: `virtual std::string getSchema() const { return std::string(); }`

## Examples

* Get a list of all registered objects:
  * Request: ```GET http://hostname:port/registry```
  * Response:
```
  {
    "namespace1/foo": ["GET", "PUT"],
    "namespace2/bar": ["PUT"]
  }
```
* Get JSON schema for a specific object:
  * Request: ```GET http://hostname:port/lexis/render/lookout/schema```
  * Response:
```
    {
        "title": "LookOut",
        "name": "LookOut",
        "type": "object",
        "properties": {
            "matrix": {
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 16,
                "maxItems": 16,
                "default": [ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 ]
            }
        },
        "required": ["matrix"],
        "additionalProperties": false
    }

```

## Implementation

* _registry_ and _schema_ are both keywords that correspond to GET requests.
  They both need to be handled by the _processGet method of the
  zeroeq::http::Server implementation.
* This mechanism should be transparent to the user of the class, and should be
  handled internally to the zeroeq::http::Server. The schema can be requested at
  any time on invocation of the getSchema() method on the registered object.

RESTful interface
=================

.. _doc-rest-authentication:

Authentication
--------------

Authentication is done through standard HTTP *Authorization* header containing
a provider name and provider-specific credentials.

Generic format of BDB authorization header is::

  Authorization: bdb-PROVIDER CREDENTIALS

Provider *noauth* can be used to specify that no authentication is to be done
and is equivalent to omitting the *Authorization* header.

Keyczar authentication provider
'''''''''''''''''''''''''''''''

*keyczar* provider can be used to sign and verify HTTP messages using
`Keyczar <http://www.keyczar.org/>`_.

To sign a message using this method, first a signable string is created
from the HTTP request. This is the concatenation of the method, query path
and values of *Content-MD5*, *Content-Type* and *Date* headers, separated
by a newline (*\\n*). The headers are sorted alphabetically, so the value of
*Content-Type* appears before the value of *Date*.

Given a sample request::

  POST /source/ HTTP/1.1
  Host: example.com
  Content-Type: application/json
  Content-MD5: f919609e57df334754cdb410c7847058
  Content-Length: nnn
  Date: Tue, 10 Jan 2012 19:03:34 GMT

  {
    "source": {
      "name": "source_name",
      "values": {
          "key1": "value1",
          "key2": "value2",
      }
    }
  }

The created signable string would be::

  POST
  /source/
  f919609e57df334754cdb410c7847058
  application/json
  Tue, 10 Jan 2012 19:03:34 GMT

The format of of the authorization header is::

  Authorization: bdb-keyczar KEYNAME:SIGNATURE

where *KEYNAME* is the name of the key the server will use to look up a key
for verifying the signature and *SIGNATURE* is the base64-encoded signature
from :meth:`keyczar.keyczar.Signer.Sign`.

On unsuccesful authentication, a *401 Unauthorized* response will be sent,
with available authentication providers in *WWW-Authenticate* header, one
provider per header::

  WWW-Authenticate: bdb-keyczar

Authorization
-------------

BDB does no authorization, every request that successfully passes
authentication is granted access to everything.

Common Request Headers
----------------------

Common Response Headers
-----------------------

JSON objects
------------

Expression representation
'''''''''''''''''''''''''

Basic expression types retain their presentation in JSON, but a symbol is
transformed to an array of literal "symbol" and the symbol name:

::

  name => ["symbol", "name"]

Since this doesn't allow to represent some lists unambiguously, lists are
prefixed with additional "list" literal:

::

  [1, "foo", "bar"] => ["list", 1, "foo", "bar"]

Operations
----------

.. _doc-rest-op-store-file:

Store file
''''''''''
**Request**
  :Synopsis: POST /file/
  :Headers: Content-Length, Content-Type
  :Body: file content

  ::

    POST /file/ HTTP/1.1
    Host: example.com
    Content-Type: application/x-hdf5
    Content-Length: nnn

    [nnn bytes of file data]

**Response**
  :Headers: Content-Length, Content-Type, Location
  :Status:
    * **201 Created** - file was successfully stored, the URI is in the
      *Location* header
    * **409 Conflict** - file already stored (not unique)
  :Body: extracted metadata (on success)

  ::

    HTTP/1.1 201 Created
    Content-Type: application/json
    Content-Length: nnn
    Location: http://example.com/file/00000000-0000-0000-0004-000000000001
    
    {
      "metadata": [
        {"path": "/", "type": "group"}
      ]
    }

.. _doc-rest-op-get-file:

Get file content
''''''''''''''''
**Request**
  :Synopsis: GET /file/{UUID of the file}/metadata

  ::

    GET /file/00000000-0000-0000-0004-000000000001/metadata HTTP/1.1
    Host: example.com

**Response**
  :Headers: Content-Length, Content-Type
  :Status:
    * **200 OK**
    * **404 NotFound**

  ::

    HTTP/1.1 200 OK
    Content-Type: application/x-hdf5
    Content-Length: nnn

    [nnn bytes of file data]

.. _doc-rest-op-get-file-metadata:

Get file metadata
'''''''''''''''''
**Request**
  :Synopsis: GET /file/{UUID of the file}/metadata

  ::

    GET /file/00000000-0000-0000-0004-000000000001/metadata HTTP/1.1
    Host: example.com

**Response**
  :Headers: Content-Length, Content-Type
  :Status:
    * **200 OK**
    * **404 NotFound**
  :Body: extracted metadata

  ::

    HTTP/1.1 200 OK
    Content-Type: application/json
    Content-Length: nnn
    
    {
      "metadata": [
        {"path": "/", "type": "group"}
      ]

    }

.. _doc-rest-op-remove-file:

Remove file
'''''''''''
**Request**
  :Synopsis: DELETE /file/{UUID of the file}

  ::

    DELETE /file/00000000-0000-0000-0004-000000000001 HTTP/1.1
    Host: example.com

**Response**
  :Status:
    * **204 NoContent** - file was removed
    * **404 NotFound** - file not found

  ::

    HTTP/1.1 204 NoContent

.. _doc-rest-op-remove-all-files:

Remove all files
''''''''''''''''
**Request**
  :Synopsis: DELETE /file/{UUID of the file}

  ::

    DELETE /file/00000000-0000-0000-0004-000000000001 HTTP/1.1
    Host: example.com

**Response**
  :Status:
    * **204 NoContent** - file was removed

  ::

    HTTP/1.1 204 NoContent

.. _doc-rest-op-query-file:

Query for files
'''''''''''''''
**Request**
  :Synopsis: POST /query/file
  :Headers: Content-Length, Content-Type
  :Body: file query

  ::

    POST /query/file HTTP/1.1
    Host: example.com
    Content-Type: application/json
    Content-Length: nnn

    {
      "filter": ["eq", ["attribute", "what/object", "string"], "PVOL"],
      "order": [["desc", ["attribute", "file:uuid", "string"]]],
      "limit": 10,
      "skip": 20
    }

**Response**
  :Headers: Content-Length, Content-Type
  :Status:
    * **200 OK**
  :Body: result rows


  ::

    HTTP/1.1 200 OK
    Content-Type: application/json
    Content-Length: nnn
    
    {
      "rows": [
        {"uuid": "00000000-0000-0000-0004-000000000001"},
        {"uuid": "00000000-0000-0000-0004-000000000002"}
      ]
    }

.. _doc-rest-op-query-attribute:

Query for metadata attributes
'''''''''''''''''''''''''''''
**Request**
  :Synopsis: POST /query/attribute
  :Headers: Content-Length, Content-Type
  :Body: attribute query

  ::

    POST /query/attribute HTTP/1.1
    Host: example.com
    Content-Type: application/json
    Content-Length: nnn

    {
      "fetch": {
        "uuid": ["list", ["symbol", "attr"],
                         "file:uuid",
                         "string"
        ]
      },
      "filter": ["list", ["symbol", "="],
                         ["list", ["symbol", "attr"],
                                  "what/object",
                                  "string"
                         ],
                         "PVOL"
      ],
      "order": [
        ["list", ["symbol", "desc"],
                 ["list", ["symbol", "attr"],
                          "file:uuid",
                          "string"
                 ]
        ]
      ],
      "group": [
        ["list", ["symbol", "attr"], "file:uuid", "string"]
      ],
      "limit": 10,
      "skip": 20,
    }


**Response**
  :Headers: Content-Length, Content-Type
  :Status:
    * **200 OK**
  :Body: result rows

  ::

    HTTP/1.1 200 OK
    Content-Type: application/json
    Content-Length: nnn
    
    {
      "rows": [
        {"uuid": "00000000-0000-0000-0004-000000000001"},
        {"uuid": "00000000-0000-0000-0004-000000000002"}
      ]
    }

.. _doc-rest-op-get-sources:

Get defined sources
'''''''''''''''''''
**Request**
  :Synopsis: GET /source/

  ::

    GET /source/ HTTP/1.1
    Host: example.com

**Response**
  :Headers: Content-Length, Content-Type
  :Status:
    * **200 OK**
  :Body: a list of sources defined in the database

  ::

    HTTP/1.1 200 OK
    Content-Type: application/json
    Content-Length: nnn
    
    {
      "sources": [
        {"name": "source1", "values": {"key1": "value"}},
        {"name": "source2", "values:" {"key2": "value"}},
      ]
    }

.. _doc-rest-op-add-source:

Add source definition
'''''''''''''''''''''

**Request**
  :Headers: Content-Length, Content-Type
  :Synopsis: POST /source/
  :Body: source object

  ::

    POST /source/ HTTP/1.1
    Host: example.com
    Content-Type: application/json
    Content-Length: nnn

    {
      "source": {
        "name": "source_name",
        "values": {
            "key1": "value1",
            "key2": "value2",
        }
      }
    }

**Response**
  :Headers: Location
  :Status:
    * **201 CREATED** - source was successfully stored, the URI is in the
      *Location* header
    * **409 CONFLICT** - source with such name is already stored

  ::

    HTTP/1.1 201 CREATED
    Location: http://example.com/source/source_name

.. _doc-rest-op-update-source:

Update source definition
''''''''''''''''''''''''
**Request**
  :Headers: Content-Length, Content-Type
  :Synopsis: PUT /source/name
  :Body: source object

  ::

    PUT /source/name HTTP/1.1
    Host: example.com
    Content-Type: application/json
    Content-Length: nnn

    {
      "source": {
        "name": "new_name",
        "values": {
            "key1": "value1",
            "key2": "value2",
        }
      }
    }

**Response**
  :Headers: Location
  :Status:
    * **204 NO CONTENT** - the source definition was successfully
      stored, the URI is in the *Location* header
    * **404 NOT FOUND** - source was not found
    * **409 CONFLICT** - source with such name is already stored

  ::

    HTTP/1.1 204 NO CONTENT
    Location: http://example.com/source/new_name

.. _doc-rest-op-remove-source:

Remove source definition
''''''''''''''''''''''''
**Request**
  :Synopsis: DELETE /source/name

  ::

    DELETE /source/name HTTP/1.1
    Host: example.com

**Response**
  :Headers: Location
  :Status:
    * **204 NO CONTENT** - the source was removed
    * **404 NOT FOUND** - source not found
    * **409 CONFLICT** - source has files associated and can't be removed

  ::

    HTTP/1.1 20O OK

.. _doc-rest-op-get-filters:

Get defined filters
'''''''''''''''''''
**Request**
  :Synopsis: GET /filter/

  ::

    GET /filter/ HTTP/1.1
    Host: example.com

**Response**
  :Headers: Content-Length, Content-Type
  :Status:
    * **200 OK**
  :Body: list of filters defined in the database.
  ::

    HTTP/1.1 200 OK
    Content-Type: application/json
    Content-Length: nnn
    
    {
      "filters": {
        "name1": {},
        "name2": {}
      }
    }

.. _doc-rest-op-get-filter:

Get a filter definition
'''''''''''''''''''''''
**Request**
  :Synopsis: GET /filter/name

  ::

    GET /filter/FOO HTTP/1.1
    Host: example.com

**Response**
  :Headers: Content-Length, Content-Type
  :Status:
    * **200 OK**
    * **404 NOT FOUND** - filter not found
  :Body: list of filters defined in the database. The keys in the
    "filters" mapping are the filter names, the values of that mapping
    are left as empty mappings, a future version might include some
    interesting metadata in that.

  ::

    HTTP/1.1 200 OK
    Content-Type: application/json
    Content-Length: nnn
    
    {
      "filter": {
        "name": "FOO",
        "expression": [
            "list", ["symbol", "="],
                    ["list", ["symbol", "attr"],
                             "what/object",
                             "string"
                    ],
                    "PVOL"
        ]
      }
    }

.. _doc-rest-op-add-filter:

Add a filter definition
'''''''''''''''''''''''
**Request**
  :Headers: Content-Length, Content-Type
  :Synopsis: POST /filter/
  :Body: the filter object to create

  ::

    POST /filter/ HTTP/1.1
    Host: example.com
    Content-Type: application/json
    Content-Length: nnn
    
    {
      "filter": {
        "name": "FOO",
        "expression": [
            "list", ["symbol", "="],
                    ["list", ["symbol", "attr"],
                             "what/object",
                             "string"
                    ],
                    "PVOL"
        ]
      }
    }

**Response**
  :Headers: Location
  :Status:
    * **201 CREATED** - filter was successfully created, the URI is in the
      *Location* header
    * **409 CONFLICT** - a filter with such a name is already stored

  ::

    HTTP/1.1 201 CREATED
    Location: http://example.com/filter/FOO

.. _doc-rest-op-remove-filter:

Remove a filter definition
''''''''''''''''''''''''''
**Request**
  :Synopsis: DELETE /source/name

  ::

    DELETE /filter/FOO HTTP/1.1
    Host: example.com

**Response**
  :Headers: Location
  :Status:
    * **204 NO CONTENT** - the filter was removed
    * **404 NOT FOUND** - the filter was not found

  ::

    HTTP/1.1 20O OK

.. _doc-rest-op-update-filter:

Update a filter definition
''''''''''''''''''''''''''
**Request**
  :Headers: Content-Length, Content-Type
  :Synopsis: PUT /filter/name
  :Body: filter object. Note that the name in the address is used, not the
         name in the filter object in the body to determine which filter
         should be updated.

  ::

    PUT /filter/FOO HTTP/1.1
    Host: example.com
    Content-Type: application/json
    Content-Length: nnn

    {
      "filter": {
        "name": "FOO",
        "expression": [
            "list", ["symbol", "="],
                    ["list", ["symbol", "attr"],
                             "what/object",
                             "string"
                    ],
                    "PVOL"
        ]
      }
    }

**Response**
  :Status:
    * **204 NO CONTENT** - the filter definition was successfully stored

  ::

    HTTP/1.1 204 NO CONTENT
    Location: http://example.com/source/new_name

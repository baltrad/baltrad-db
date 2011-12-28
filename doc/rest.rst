RESTful interface
=================

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

  [1, "foo", "bar"] => ["list, 1, "foo", "bar"]

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
      "filter": ["eq" ["attribute", "what/object", "string"] "PVOL"],
      "order": [["desc" ["attribute", "file:uuid", "string"]]],
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
      "fetch": {"uuid": ["attribute", "file:uuid", "string"]},
      "filter": ["eq" ["attribute", "what/object", "string"] "PVOL"],
      "order": [["desc" ["attribute", "file:uuid", "string"]]],
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
  :Body: extracted metadata

  ::

    HTTP/1.1 200 OK
    Content-Type: application/json
    Content-Length: nnn
    
    {
      "sources": [
        {"name": "source1", "key1": "value"},
        {"name": "source2", "key2": "value"},
      ]
    }

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
  :Body: extracted metadata

  ::

    HTTP/1.1 201 CREATED
    Location: http://example.com/source/source_name

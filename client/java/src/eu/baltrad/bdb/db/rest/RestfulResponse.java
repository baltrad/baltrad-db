package eu.baltrad.bdb.db.rest;

import eu.baltrad.bdb.oh5.Metadata;
import eu.baltrad.bdb.oh5.Source;

import org.apache.commons.io.IOUtils;
import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.util.EntityUtils;

import org.codehaus.jackson.JsonNode;

import java.io.InputStream;
import java.io.IOException;

import java.util.*;

/**
 * Wrapper for server responses
 */
public final class RestfulResponse {
  HttpResponse httpResponse;
  JsonUtil jsonUtil;

  public RestfulResponse(HttpResponse httpResponse) {
    this.httpResponse = httpResponse;
    this.jsonUtil = new JsonUtil();
  }
  
  /**
   * get the HTTP status code for this response
   */
  public int getStatusCode() {
    return httpResponse.getStatusLine().getStatusCode();
  }
  
  /**
   * get the root json node of the response body
   */
  public JsonNode getJsonContent() {
    InputStream is = getContentStream();
    try {
      return jsonUtil.jsonFromStream(is);
    } finally {
      IOUtils.closeQuietly(is);
    }

  }
  
  /**
   * get a stream of the response body
   */
  public InputStream getContentStream() {
    HttpEntity entity = httpResponse.getEntity();
    if (entity == null) {
      throw new RuntimeException("response is missing body");
    }
    try {
      return entity.getContent();
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
  }
  
  public Metadata getMetadata() {
    JsonNode metadataNode = getJsonContent().get("metadata");
    return jsonUtil.createOh5Metadata(metadataNode);
  }

  public List<Map<String, Object>> getResultRows() {
    JsonNode rowsNode = getJsonContent().get("rows");
    List<Map<String, Object>> result = new ArrayList<Map<String, Object>>();
    for (JsonNode rowNode : rowsNode) {
      Map<String, Object> row = new HashMap<String, Object>();
      Iterator<Map.Entry<String, JsonNode>> fieldIterator = rowNode.getFields();
      while (fieldIterator.hasNext()) {
        Map.Entry<String, JsonNode> field = fieldIterator.next();
        Object value = getObjectFromValueNode(field.getValue());
        row.put(field.getKey(), value);
      }
      result.add(row);
    }
    return result;
  }

  public List<Source> getSources() {
    JsonNode sourcesNode = getJsonContent().get("sources");

    List<Source> result = new ArrayList<Source>();
    for (JsonNode sourceNode : sourcesNode) {
      result.add(getSourceFromNode(sourceNode));
    }
    return result;
  }

  protected Source getSourceFromNode(JsonNode node) {
    Source result = new Source();
    result.setName(node.get("name").getTextValue());
    Iterator<Map.Entry<String, JsonNode>> fieldIterator = node.get("values").getFields();
    while (fieldIterator.hasNext()) {
      Map.Entry<String, JsonNode> field = fieldIterator.next();
      result.put(field.getKey(), field.getValue().getTextValue());
    }
    return result;
  }

  protected Object getObjectFromValueNode(JsonNode node) {
    if (!node.isValueNode()) {
      throw new RuntimeException("not a value node");
    }
    
    if (node.isTextual()) {
      return node.getTextValue();
    } else if (node.isIntegralNumber()) {
      return new Long(node.getValueAsLong());
    } else if (node.isFloatingPointNumber()) {
      return new Double(node.getValueAsDouble());
    } else if (node.isBoolean()) {
      return new Boolean(node.getValueAsBoolean());
    } else if (node.isNull()) {
      return null;
    }

    throw new RuntimeException("unhandled value: " + node.getValueAsText());
  }
  

  /**
   * close this response instance.
   */
  public void close() {
    try {
      // ensure entity is consumed to release low-level resources
      EntityUtils.consume(httpResponse.getEntity());
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
  }
}

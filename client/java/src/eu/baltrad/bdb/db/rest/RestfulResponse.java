package eu.baltrad.bdb.db.rest;

import eu.baltrad.bdb.oh5.Metadata;
import eu.baltrad.bdb.oh5.Source;

import org.apache.commons.io.IOUtils;
import org.apache.hc.core5.http.HttpEntity;
import org.apache.hc.core5.http.ClassicHttpResponse;
import org.apache.hc.core5.http.io.entity.EntityUtils;

import com.fasterxml.jackson.databind.JsonNode;

import java.io.InputStream;
import java.io.IOException;

import java.util.*;

/**
 * Wrapper for server responses
 */
public final class RestfulResponse {
  ClassicHttpResponse httpResponse;
  JsonUtil jsonUtil;

  public RestfulResponse(ClassicHttpResponse httpResponse) {
    this.httpResponse = httpResponse;
    this.jsonUtil = new JsonUtil();
  }
  
  /**
   * get the HTTP status code for this response
   */
  public int getStatusCode() {
    return httpResponse.getCode();
  }

  public String getReason() {
    return httpResponse.getReasonPhrase();
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
      Iterator<Map.Entry<String, JsonNode>> fieldIterator = rowNode.fields();
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

  public Source getSource() {
    JsonNode sourceNode = getJsonContent().get("source");
    return getSourceFromNode(sourceNode);
  }

  protected Source getSourceFromNode(JsonNode node) {
    Source result = new Source();
    result.setName(node.get("name").asText());
    if (node.has("parent") && !node.get("parent").isNull()) {
      result.setParent(node.get("parent").asText());
    }
    Iterator<Map.Entry<String, JsonNode>> fieldIterator = node.get("values").fields();
    while (fieldIterator.hasNext()) {
      Map.Entry<String, JsonNode> field = fieldIterator.next();
      result.put(field.getKey(), field.getValue().asText());
    }
    return result;
  }

  protected Object getObjectFromValueNode(JsonNode node) {
    if (!node.isValueNode()) {
      throw new RuntimeException("not a value node");
    }
    
    if (node.isTextual()) {
      return node.asText();
    } else if (node.isIntegralNumber()) {
      return Long.valueOf(node.asLong());
    } else if (node.isFloatingPointNumber()) {
      return Double.valueOf(node.asDouble());
    } else if (node.isBoolean()) {
      return Boolean.valueOf(node.asBoolean());
    } else if (node.isNull()) {
      return null;
    }

    throw new RuntimeException("unhandled value: " + node.asText());
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

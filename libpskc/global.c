/*
 * global.c - implementation of library global functions
 * Copyright (C) 2012 Simon Josefsson
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <config.h>

#include "pskc.h"

#include <string.h>		/* strverscmp */
#include <libxml/parser.h>	/* xmlInitParser */
#include <libxml/xmlschemas.h>

extern xmlSchemaValidCtxtPtr _pskc_schema_validctxt;

/* RFC 6030 with errata fix and no references to xenc/ds. */
const char *pskc_schema_str =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
  "<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"\n"
  "     xmlns:pskc=\"urn:ietf:params:xml:ns:keyprov:pskc\"\n"
  "     targetNamespace=\"urn:ietf:params:xml:ns:keyprov:pskc\"\n"
  "     elementFormDefault=\"qualified\"\n"
  "     attributeFormDefault=\"unqualified\">\n"
  "     <xs:import namespace=\"http://www.w3.org/XML/1998/namespace\"/>\n"
  "     <xs:complexType name=\"KeyContainerType\">\n"
  "          <xs:sequence>\n"
  "               <xs:element name=\"MACMethod\"\n"
  "                    type=\"pskc:MACMethodType\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"KeyPackage\"\n"
  "                    type=\"pskc:KeyPackageType\" maxOccurs=\"unbounded\"/>\n"
  "               <xs:element name=\"Extensions\"\n"
  "                    type=\"pskc:ExtensionsType\"\n"
  "                    minOccurs=\"0\" maxOccurs=\"unbounded\"/>\n"
  "          </xs:sequence>\n"
  "          <xs:attribute name=\"Version\"\n"
  "               type=\"pskc:VersionType\" use=\"required\"/>\n"
  "          <xs:attribute name=\"Id\"\n"
  "               type=\"xs:ID\" use=\"optional\"/>\n"
  "     </xs:complexType>\n"
  "     <xs:simpleType name=\"VersionType\" final=\"restriction\">\n"
  "          <xs:restriction base=\"xs:string\">\n"
  "               <xs:pattern value=\"\\d{1,2}\\.\\d{1,3}\"/>\n"
  "          </xs:restriction>\n"
  "     </xs:simpleType>\n"
  "     <xs:complexType name=\"KeyType\">\n"
  "          <xs:sequence>\n"
  "               <xs:element name=\"Issuer\"\n"
  "                    type=\"xs:string\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"AlgorithmParameters\"\n"
  "                    type=\"pskc:AlgorithmParametersType\"\n"
  "                    minOccurs=\"0\"/>\n"
  "               <xs:element name=\"KeyProfileId\"\n"
  "                    type=\"xs:string\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"KeyReference\"\n"
  "                    type=\"xs:string\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"FriendlyName\"\n"
  "                    type=\"xs:string\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"Data\"\n"
  "                    type=\"pskc:KeyDataType\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"UserId\"\n"
  "                    type=\"xs:string\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"Policy\"\n"
  "                    type=\"pskc:PolicyType\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"Extensions\"\n"
  "                    type=\"pskc:ExtensionsType\" minOccurs=\"0\"\n"
  "                    maxOccurs=\"unbounded\"/>\n"
  "          </xs:sequence>\n"
  "          <xs:attribute name=\"Id\"\n"
  "               type=\"xs:string\" use=\"required\"/>\n"
  "          <xs:attribute name=\"Algorithm\"\n"
  "               type=\"pskc:KeyAlgorithmType\" use=\"optional\"/>\n"
  "     </xs:complexType>\n"
  "     <xs:complexType name=\"PolicyType\">\n"
  "          <xs:sequence>\n"
  "               <xs:element name=\"StartDate\"\n"
  "                    type=\"xs:dateTime\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"ExpiryDate\"\n"
  "                    type=\"xs:dateTime\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"PINPolicy\"\n"
  "                    type=\"pskc:PINPolicyType\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"KeyUsage\"\n"
  "                    type=\"pskc:KeyUsageType\"\n"
  "                    minOccurs=\"0\" maxOccurs=\"unbounded\"/>\n"
  "               <xs:element name=\"NumberOfTransactions\"\n"
  "                    type=\"xs:nonNegativeInteger\" minOccurs=\"0\"/>\n"
  "               <xs:any namespace=\"##other\"\n"
  "                    minOccurs=\"0\" maxOccurs=\"unbounded\"/>\n"
  "          </xs:sequence>\n"
  "     </xs:complexType>\n"
  "     <xs:complexType name=\"KeyDataType\">\n"
  "          <xs:sequence>\n"
  "               <xs:element name=\"Secret\"\n"
  "                    type=\"pskc:binaryDataType\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"Counter\"\n"
  "                    type=\"pskc:longDataType\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"Time\"\n"
  "                    type=\"pskc:intDataType\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"TimeInterval\"\n"
  "                    type=\"pskc:intDataType\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"TimeDrift\"\n"
  "                    type=\"pskc:intDataType\" minOccurs=\"0\"/>\n"
  "               <xs:any namespace=\"##other\"\n"
  "                    processContents=\"lax\"\n"
  "                    minOccurs=\"0\" maxOccurs=\"unbounded\"/>\n"
  "          </xs:sequence>\n"
  "     </xs:complexType>\n"
  "     <xs:complexType name=\"binaryDataType\">\n"
  "          <xs:sequence>\n"
  "               <xs:choice>\n"
  "                    <xs:element name=\"PlainValue\"\n"
  "                         type=\"xs:base64Binary\"/>\n"
  "               </xs:choice>\n"
  "               <xs:element name=\"ValueMAC\"\n"
  "                    type=\"xs:base64Binary\" minOccurs=\"0\"/>\n"
  "          </xs:sequence>\n"
  "     </xs:complexType>\n"
  "     <xs:complexType name=\"intDataType\">\n"
  "          <xs:sequence>\n"
  "               <xs:choice>\n"
  "                    <xs:element name=\"PlainValue\" type=\"xs:int\"/>\n"
  "               </xs:choice>\n"
  "               <xs:element name=\"ValueMAC\"\n"
  "                    type=\"xs:base64Binary\" minOccurs=\"0\"/>\n"
  "          </xs:sequence>\n"
  "     </xs:complexType>\n"
  "     <xs:complexType name=\"stringDataType\">\n"
  "          <xs:sequence>\n"
  "               <xs:choice>\n"
  "                    <xs:element name=\"PlainValue\" type=\"xs:string\"/>\n"
  "               </xs:choice>\n"
  "               <xs:element name=\"ValueMAC\"\n"
  "                    type=\"xs:base64Binary\" minOccurs=\"0\"/>\n"
  "          </xs:sequence>\n"
  "     </xs:complexType>\n"
  "     <xs:complexType name=\"longDataType\">\n"
  "          <xs:sequence>\n"
  "               <xs:choice>\n"
  "                    <xs:element name=\"PlainValue\" type=\"xs:long\"/>\n"
  "               </xs:choice>\n"
  "               <xs:element name=\"ValueMAC\"\n"
  "                    type=\"xs:base64Binary\" minOccurs=\"0\"/>\n"
  "          </xs:sequence>\n"
  "     </xs:complexType>\n"
  "     <xs:complexType name=\"PINPolicyType\">\n"
  "          <xs:attribute name=\"PINKeyId\"\n"
  "               type=\"xs:string\" use=\"optional\"/>\n"
  "          <xs:attribute name=\"PINUsageMode\"\n"
  "               type=\"pskc:PINUsageModeType\"/>\n"
  "          <xs:attribute name=\"MaxFailedAttempts\"\n"
  "               type=\"xs:unsignedInt\" use=\"optional\"/>\n"
  "          <xs:attribute name=\"MinLength\"\n"
  "               type=\"xs:unsignedInt\" use=\"optional\"/>\n"
  "          <xs:attribute name=\"MaxLength\"\n"
  "               type=\"xs:unsignedInt\" use=\"optional\"/>\n"
  "          <xs:attribute name=\"PINEncoding\"\n"
  "               type=\"pskc:ValueFormatType\" use=\"optional\"/>\n"
  "          <xs:anyAttribute namespace=\"##other\"/>\n"
  "     </xs:complexType>\n"
  "     <xs:simpleType name=\"PINUsageModeType\">\n"
  "          <xs:restriction base=\"xs:string\">\n"
  "               <xs:enumeration value=\"Local\"/>\n"
  "               <xs:enumeration value=\"Prepend\"/>\n"
  "               <xs:enumeration value=\"Append\"/>\n"
  "               <xs:enumeration value=\"Algorithmic\"/>\n"
  "          </xs:restriction>\n"
  "     </xs:simpleType>\n"
  "     <xs:simpleType name=\"KeyUsageType\">\n"
  "          <xs:restriction base=\"xs:string\">\n"
  "               <xs:enumeration value=\"OTP\"/>\n"
  "               <xs:enumeration value=\"CR\"/>\n"
  "               <xs:enumeration value=\"Encrypt\"/>\n"
  "               <xs:enumeration value=\"Integrity\"/>\n"
  "               <xs:enumeration value=\"Verify\"/>\n"
  "               <xs:enumeration value=\"Unlock\"/>\n"
  "               <xs:enumeration value=\"Decrypt\"/>\n"
  "               <xs:enumeration value=\"KeyWrap\"/>\n"
  "               <xs:enumeration value=\"Unwrap\"/>\n"
  "               <xs:enumeration value=\"Derive\"/>\n"
  "               <xs:enumeration value=\"Generate\"/>\n"
  "          </xs:restriction>\n"
  "     </xs:simpleType>\n"
  "     <xs:complexType name=\"DeviceInfoType\">\n"
  "          <xs:sequence>\n"
  "               <xs:element name=\"Manufacturer\"\n"
  "                    type=\"xs:string\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"SerialNo\"\n"
  "                    type=\"xs:string\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"Model\"\n"
  "                    type=\"xs:string\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"IssueNo\"\n"
  "                    type=\"xs:string\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"DeviceBinding\"\n"
  "                    type=\"xs:string\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"StartDate\"\n"
  "                    type=\"xs:dateTime\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"ExpiryDate\"\n"
  "                    type=\"xs:dateTime\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"UserId\"\n"
  "                    type=\"xs:string\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"Extensions\"\n"
  "                    type=\"pskc:ExtensionsType\" minOccurs=\"0\"\n"
  "                    maxOccurs=\"unbounded\"/>\n"
  "          </xs:sequence>\n"
  "     </xs:complexType>\n"
  "     <xs:complexType name=\"CryptoModuleInfoType\">\n"
  "          <xs:sequence>\n"
  "               <xs:element name=\"Id\" type=\"xs:string\"/>\n"
  "               <xs:element name=\"Extensions\"\n"
  "                    type=\"pskc:ExtensionsType\" minOccurs=\"0\"\n"
  "                    maxOccurs=\"unbounded\"/>\n"
  "          </xs:sequence>\n"
  "     </xs:complexType>\n"
  "     <xs:complexType name=\"KeyPackageType\">\n"
  "          <xs:sequence>\n"
  "               <xs:element name=\"DeviceInfo\"\n"
  "                    type=\"pskc:DeviceInfoType\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"CryptoModuleInfo\"\n"
  "                    type=\"pskc:CryptoModuleInfoType\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"Key\"\n"
  "                    type=\"pskc:KeyType\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"Extensions\"\n"
  "                    type=\"pskc:ExtensionsType\" minOccurs=\"0\"\n"
  "                    maxOccurs=\"unbounded\"/>\n"
  "          </xs:sequence>\n"
  "     </xs:complexType>\n"
  "     <xs:complexType name=\"AlgorithmParametersType\">\n"
  "          <xs:sequence>\n"
  "               <xs:element name=\"Suite\" type=\"xs:string\" minOccurs=\"0\"/>\n"
  "               <xs:element name=\"ChallengeFormat\" minOccurs=\"0\">\n"
  "                    <xs:complexType>\n"
  "                         <xs:attribute name=\"Encoding\"\n"
  "                              type=\"pskc:ValueFormatType\"\n"
  "                                                      use=\"required\"/>\n"
  "                         <xs:attribute name=\"Min\"\n"
  "                              type=\"xs:unsignedInt\" use=\"required\"/>\n"
  "                         <xs:attribute name=\"Max\"\n"
  "                              type=\"xs:unsignedInt\" use=\"required\"/>\n"
  "                         <xs:attribute name=\"CheckDigits\"\n"
  "                              type=\"xs:boolean\" default=\"false\"/>\n"
  "                    </xs:complexType>\n"
  "               </xs:element>\n"
  "               <xs:element name=\"ResponseFormat\" minOccurs=\"0\">\n"
  "                    <xs:complexType>\n"
  "                         <xs:attribute name=\"Encoding\"\n"
  "                              type=\"pskc:ValueFormatType\"\n"
  "                                                      use=\"required\"/>\n"
  "                         <xs:attribute name=\"Length\"\n"
  "                              type=\"xs:unsignedInt\" use=\"required\"/>\n"
  "                         <xs:attribute name=\"CheckDigits\"\n"
  "                              type=\"xs:boolean\" default=\"false\"/>\n"
  "                    </xs:complexType>\n"
  "               </xs:element>\n"
  "               <xs:element name=\"Extensions\"\n"
  "                    type=\"pskc:ExtensionsType\" minOccurs=\"0\"\n"
  "                    maxOccurs=\"unbounded\"/>\n"
  "          </xs:sequence>\n"
  "     </xs:complexType>\n"
  "     <xs:complexType name=\"ExtensionsType\">\n"
  "          <xs:sequence>\n"
  "               <xs:any namespace=\"##other\"\n"
  "                    processContents=\"lax\" maxOccurs=\"unbounded\"/>\n"
  "          </xs:sequence>\n"
  "          <xs:attribute name=\"definition\"\n"
  "               type=\"xs:anyURI\" use=\"optional\"/>\n"
  "     </xs:complexType>\n"
  "     <xs:simpleType name=\"KeyAlgorithmType\">\n"
  "          <xs:restriction base=\"xs:anyURI\"/>\n"
  "     </xs:simpleType>\n"
  "     <xs:simpleType name=\"ValueFormatType\">\n"
  "          <xs:restriction base=\"xs:string\">\n"
  "               <xs:enumeration value=\"DECIMAL\"/>\n"
  "               <xs:enumeration value=\"HEXADECIMAL\"/>\n"
  "               <xs:enumeration value=\"ALPHANUMERIC\"/>\n"
  "               <xs:enumeration value=\"BASE64\"/>\n"
  "               <xs:enumeration value=\"BINARY\"/>\n"
  "          </xs:restriction>\n"
  "     </xs:simpleType>\n"
  "     <xs:complexType name=\"MACMethodType\">\n"
  "           <xs:sequence>\n"
  "                  <xs:choice>\n"
  "                        <xs:element name=\"MACKeyReference\"\n"
  "                                type=\"xs:string\" minOccurs=\"0\"/>\n"
  "                        </xs:choice>\n"
  "                        <xs:any namespace=\"##other\"\n"
  "           processContents=\"lax\" minOccurs=\"0\" maxOccurs=\"unbounded\"/>\n"
  "       </xs:sequence>\n"
  "       <xs:attribute name=\"Algorithm\" type=\"xs:anyURI\" use=\"required\"/>\n"
  "        </xs:complexType>\n"
  "     <xs:element name=\"KeyContainer\"\n"
  "          type=\"pskc:KeyContainerType\"/>\n"
  "</xs:schema>\n";

xmlDocPtr schema_doc = NULL;
xmlSchemaParserCtxtPtr parser_ctxt;
xmlSchemaPtr schema;
xmlSchemaValidCtxtPtr _pskc_schema_validctxt;

/**
 * pskc_global_init:
 *
 * This function initializes the PSKC library.  Every user of this
 * library needs to call this function before using other functions.
 * You should call pskc_global_done() when use of the PSKC library is
 * no longer needed.
 *
 * Returns: On success, %PSKC_OK (zero) is returned, otherwise an
 *   error code is returned.
 **/
int
pskc_global_init (void)
{
  xmlInitParser ();

  schema_doc = xmlReadMemory (pskc_schema_str, strlen (pskc_schema_str),
			      NULL, NULL, XML_PARSE_NONET);
  if (schema_doc == NULL)
    return PSKC_XML_PARSE_ERROR;

  parser_ctxt = xmlSchemaNewDocParserCtxt (schema_doc);
  if (parser_ctxt == NULL)
    return PSKC_XML_PARSE_ERROR;

  schema = xmlSchemaParse (parser_ctxt);
  if (schema == NULL)
    {
      xmlSchemaFreeParserCtxt (parser_ctxt);
      return PSKC_XML_PARSE_ERROR;
    }

  _pskc_schema_validctxt = xmlSchemaNewValidCtxt (schema);
  if (_pskc_schema_validctxt == NULL)
    {
      xmlSchemaFree (schema);
      xmlSchemaFreeParserCtxt (parser_ctxt);
      return PSKC_XML_PARSE_ERROR;
    }

  return PSKC_OK;
}

/**
 * pskc_global_done:
 *
 * This function deinitializes the PSKC library, which were
 * initialized using pskc_global_init().  After calling this function,
 * no other PSKC library function may be called except for to
 * re-initialize the library using pskc_global_init().
 *
 * Returns: On success, %PSKC_OK (zero) is returned, otherwise an
 *   error code is returned.
 **/
int
pskc_global_done (void)
{
  xmlSchemaFreeValidCtxt (_pskc_schema_validctxt);
  xmlSchemaFree (schema);
  xmlSchemaFreeParserCtxt (parser_ctxt);
  xmlCleanupParser ();
  xmlMemoryDump ();
  return PSKC_OK;
}

pskc_log_func _pskc_log_func;

/**
 * pskc_global_log:
 * @log_func: new global #pskc_log_func log function to use.
 *
 * Enable global debug logging function.  The function will be invoked
 * to print various debugging information.
 *
 * @pskc_log_func is of the form,
 * void (*pskc_log_func) (pskc_t *container, pskc_key_t *keypackage,
 * const char *format, ...);
 *
 * The container and keypackage variables may be NULL if they are not
 * relevant for the debug information printed.
 **/
void
pskc_global_log (pskc_log_func log_func)
{
  _pskc_log_func = log_func;
}

/**
 * pskc_check_version:
 * @req_version: version string to compare with, or NULL.
 *
 * Check PSKC library version.
 *
 * See %PSKC_VERSION for a suitable @req_version string.
 *
 * This function is one of few in the library that can be used without
 * a successful call to pskc_global_init().
 *
 * Return value: Check that the version of the library is at
 *   minimum the one given as a string in @req_version and return the
 *   actual version string of the library; return NULL if the
 *   condition is not met.  If NULL is passed to this function no
 *   check is done and only the version string is returned.
 **/
const char *
pskc_check_version (const char *req_version)
{
  if (!req_version || strverscmp (req_version, PSKC_VERSION) <= 0)
    return PSKC_VERSION;

  return NULL;
}

/**
 * pskc_free:
 * @ptr: memory region to deallocate, or %NULL.
 *
 * Deallocates memory region by calling free().  If @ptr is %NULL no
 * operation is performed.
 *
 * This function is necessary on Windows, where different parts of the
 * same application may use different memory heaps.
 **/
void
pskc_free (void *ptr)
{
  free (ptr);
}

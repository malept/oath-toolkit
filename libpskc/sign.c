/*
 * sign.c - Sign PSKC data with X.509 key/cert.
 * Copyright (C) 2012-2013 Simon Josefsson
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

#include <pskc/pskc.h>

#define INTERNAL_NEED_PSKC_STRUCT
#include "internal.h"
#include <xmlsec/xmlsec.h>
#include <xmlsec/crypto.h>
#include <xmlsec/xmltree.h>
#include <xmlsec/xmldsig.h>
#include <xmlsec/templates.h>

/**
 * pskc_sign_x509:
 * @container: a #pskc_t handle, from pskc_init().
 * @key_file: filename of file containing private key.
 * @cert_file: filename of file containing corresponding X.509 certificate.
 *
 * Sign PSKC data using X.509 certificate and private key.
 *
 * Returns: On success, %PSKC_OK (zero) is returned, or an error code.
 */
int
pskc_sign_x509 (pskc_t * container,
		const char *key_file, const char *cert_file)
{
  xmlNodePtr signNode;
  xmlNodePtr keyInfoNode = NULL;
  xmlNodePtr refNode = NULL;
  xmlSecDSigCtxPtr dsigCtx = NULL;

  pskc_build_xml (container, NULL, NULL);

  /* create signature template for RSA-SHA1 enveloped signature */
  signNode = xmlSecTmplSignatureCreate (container->xmldoc,
					xmlSecTransformExclC14NId,
					xmlSecTransformRsaSha1Id, NULL);
  if (signNode == NULL)
    {
      _pskc_debug ("xmlSecTmplSignatureCreateNsPref failed");
      return PSKC_XMLSEC_ERROR;
    }

  /* add <dsig:Signature/> node to the doc */
  xmlAddChild (xmlDocGetRootElement (container->xmldoc), signNode);

  /* add reference */
  refNode = xmlSecTmplSignatureAddReference (signNode, xmlSecTransformSha1Id,
					     NULL, NULL, NULL);
  if (refNode == NULL)
    {
      _pskc_debug ("xmlSecTmplSignatureAddReference failed");
      return PSKC_XMLSEC_ERROR;
    }

  /* add enveloped transform */
  if (xmlSecTmplReferenceAddTransform (refNode,
				       xmlSecTransformEnvelopedId) == NULL)
    {
      _pskc_debug ("xmlSecTmplReferenceAddTransform failed");
      return PSKC_XMLSEC_ERROR;
    }

  /* add <dsig:KeyInfo/> and <dsig:X509Data/> */
  keyInfoNode = xmlSecTmplSignatureEnsureKeyInfo (signNode, NULL);
  if (keyInfoNode == NULL)
    {
      _pskc_debug ("xmlSecTmplSignatureEnsureKeyInfo failed");
      return PSKC_XMLSEC_ERROR;
    }

  if (xmlSecTmplKeyInfoAddX509Data (keyInfoNode) == NULL)
    {
      _pskc_debug ("xmlSecTmplKeyInfoAddX509Data failed");
      return PSKC_XMLSEC_ERROR;
    }

  /* create signature context, we don't need keys manager in this example */
  dsigCtx = xmlSecDSigCtxCreate (NULL);
  if (dsigCtx == NULL)
    {
      _pskc_debug ("xmlSecDSigCtxCreate failed");
      return PSKC_XMLSEC_ERROR;
    }

  /* load private key, assuming that there is not password */
  dsigCtx->signKey = xmlSecCryptoAppKeyLoad (key_file,
					     xmlSecKeyDataFormatPem,
					     NULL, NULL, NULL);
  if (dsigCtx->signKey == NULL)
    {
      _pskc_debug ("xmlSecCryptoAppKeyLoad failed");
      return PSKC_XMLSEC_ERROR;
    }

  /* load certificate and add to the key */
  if (xmlSecCryptoAppKeyCertLoad
      (dsigCtx->signKey, cert_file, xmlSecKeyDataFormatCertPem) < 0)
    {
      _pskc_debug ("xmlSecCryptoAppKeyCertLoad failed");
      return PSKC_XMLSEC_ERROR;
    }

  /* set key name to the file name, this is just an example! */
  if (xmlSecKeySetName (dsigCtx->signKey, BAD_CAST key_file) < 0)
    {
      _pskc_debug ("xmlSecKeySetName failed");
      return PSKC_XMLSEC_ERROR;
    }

  /* sign the template */
  if (xmlSecDSigCtxSign (dsigCtx, signNode) < 0)
    {
      _pskc_debug ("xmlSecDSigCtxSign failed");
      return PSKC_XMLSEC_ERROR;
    }

  return PSKC_OK;
}

static int
verify (pskc_t * container, xmlSecKeysMngrPtr mngr,
	xmlSecDSigCtxPtr dsigCtx, const char *cert_file, int *valid_signature)
{
  xmlNodePtr node = NULL;

  node = xmlSecFindNode (xmlDocGetRootElement (container->xmldoc),
			 xmlSecNodeSignature, xmlSecDSigNs);
  if (node == NULL)
    {
      _pskc_debug ("xmlSecFindNode failed");
      return PSKC_XMLSEC_ERROR;
    }

  if (xmlSecCryptoAppDefaultKeysMngrInit (mngr) < 0)
    {
      _pskc_debug ("xmlSecCryptoAppDefaultKeysMngrInit failed");
      return PSKC_XMLSEC_ERROR;
    }

  if (xmlSecCryptoAppKeysMngrCertLoad (mngr, cert_file,
				       xmlSecKeyDataFormatPem,
				       xmlSecKeyDataTypeTrusted) < 0)
    {
      _pskc_debug ("xmlSecCryptoAppKeysMngrCertLoad failed");
      return PSKC_XMLSEC_ERROR;
    }

  if (xmlSecDSigCtxVerify (dsigCtx, node) < 0)
    {
      _pskc_debug ("xmlSecDSigCtxVerify failed");
      return PSKC_XMLSEC_ERROR;
    }

  if (dsigCtx->status == xmlSecDSigStatusSucceeded)
    *valid_signature = 1;
  else
    *valid_signature = 0;

  return PSKC_OK;
}

/**
 * pskc_verify_x509crt:
 * @container: a #pskc_t handle, from pskc_init().
 * @cert_file: filename of file containing trusted X.509 certificate.
 * @valid_signature: output variable with result of verification.
 *
 * Verify signature in PSKC data against trusted X.509 certificate.
 *
 * Returns: On success, %PSKC_OK (zero) is returned, or an error code.
 */
int
pskc_verify_x509crt (pskc_t * container, const char *cert_file,
		     int *valid_signature)
{
  xmlSecKeysMngrPtr mngr = NULL;
  xmlSecDSigCtxPtr dsigCtx = NULL;
  int rc;

  mngr = xmlSecKeysMngrCreate ();
  if (mngr == NULL)
    {
      _pskc_debug ("xmlSecKeysMngrCreate failed");
      return PSKC_XMLSEC_ERROR;
    }

  dsigCtx = xmlSecDSigCtxCreate (mngr);
  if (dsigCtx == NULL)
    {
      _pskc_debug ("xmlSecDSigCtxCreate failed");
      xmlSecKeysMngrDestroy (mngr);
      return PSKC_XMLSEC_ERROR;
    }

  rc = verify (container, mngr, dsigCtx, cert_file, valid_signature);

  xmlSecDSigCtxDestroy (dsigCtx);
  xmlSecKeysMngrDestroy (mngr);

  return rc;
}

/*
 * sign.c - Sign PSKC data with X.509 key/cert.
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

#include <pskc/pskc.h>

#define INTERNAL_NEED_PSKC_STRUCT
#include "internal.h"
#include <xmlsec/xmlsec.h>
#include <xmlsec/crypto.h>
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
  signNode = xmlSecTmplSignatureCreateNsPref (container->xmldoc,
					      xmlSecTransformExclC14NId,
					      xmlSecTransformRsaSha1Id, NULL,
					      BAD_CAST "ds");
  if (signNode == NULL)
    {
      _pskc_debug ("xmlSecTmplSignatureCreateNsPref failed\n");
      return PSKC_XMLSEC_ERROR;
    }

  /* add <dsig:Signature/> node to the doc */
  xmlAddChild (xmlDocGetRootElement (container->xmldoc), signNode);

  /* add reference */
  refNode = xmlSecTmplSignatureAddReference (signNode, xmlSecTransformSha1Id,
					     NULL, NULL, NULL);
  if (refNode == NULL)
    {
      _pskc_debug ("xmlSecTmplSignatureAddReference failed\n");
      return PSKC_XMLSEC_ERROR;
    }

  /* add enveloped transform */
  if (xmlSecTmplReferenceAddTransform (refNode,
				       xmlSecTransformEnvelopedId) == NULL)
    {
      _pskc_debug ("xmlSecTmplReferenceAddTransform failed\n");
      return PSKC_XMLSEC_ERROR;
    }

  /* add <dsig:KeyInfo/> and <dsig:X509Data/> */
  keyInfoNode = xmlSecTmplSignatureEnsureKeyInfo (signNode, NULL);
  if (keyInfoNode == NULL)
    {
      _pskc_debug ("xmlSecTmplSignatureEnsureKeyInfo failed\n");
      return PSKC_XMLSEC_ERROR;
    }

  if (xmlSecTmplKeyInfoAddX509Data (keyInfoNode) == NULL)
    {
      _pskc_debug ("xmlSecTmplKeyInfoAddX509Data failed\n");
      return PSKC_XMLSEC_ERROR;
    }

  /* create signature context, we don't need keys manager in this example */
  dsigCtx = xmlSecDSigCtxCreate (NULL);
  if (dsigCtx == NULL)
    {
      _pskc_debug ("xmlSecDSigCtxCreate failed\n");
      return PSKC_XMLSEC_ERROR;
    }

  /* load private key, assuming that there is not password */
  dsigCtx->signKey = xmlSecCryptoAppKeyLoad (key_file,
					     xmlSecKeyDataFormatPem,
					     NULL, NULL, NULL);
  if (dsigCtx->signKey == NULL)
    {
      _pskc_debug ("xmlSecCryptoAppKeyLoad failed\n");
      return PSKC_XMLSEC_ERROR;
    }

  /* load certificate and add to the key */
  if (xmlSecCryptoAppKeyCertLoad
      (dsigCtx->signKey, cert_file, xmlSecKeyDataFormatCertPem) < 0)
    {
      _pskc_debug ("xmlSecCryptoAppKeyCertLoad failed\n");
      return PSKC_XMLSEC_ERROR;
    }

  /* sign the template */
  if (xmlSecDSigCtxSign (dsigCtx, signNode) < 0)
    {
      _pskc_debug ("xmlSecDSigCtxSign failed\n");
      return PSKC_XMLSEC_ERROR;
    }

  return PSKC_OK;
}

<?php
// $Id$
/**
 * @file
 * Implements Zimbra e-mail accounts provisioning class.
 */

//////////////////////////////////////////////////////////////////////////////

require_once('SOAP/Client.php');

//////////////////////////////////////////////////////////////////////////////

class ZimbraSoapClient {

  var $url;
  var $namespace;
  var $context;
  var $options;

  var $header;
  var $client;

  function ZimbraSoapClient($url) {
    $this->url = $url;
    $this->namespace = 'urn:zimbraAdmin';
    $this->context = '{urn:zimbra}context';
    $this->options = array('namespace' => $this->namespace, 'trace' => 1, 'timeout' => ZIMBRA_TIMEOUT);

    $this->client = new SOAP_Client($this->url);
    if (preg_match('/^https/', $url)) {
      $this->client->setOpt('curl', CURLOPT_SSL_VERIFYPEER, ZIMBRA_CURLOPT_SSL_VERIFYPEER);
      $this->client->setOpt('curl', CURLOPT_SSL_VERIFYHOST, ZIMBRA_CURLOPT_SSL_VERIFYHOST);
    }
    $this->client->setTrace(TRUE);
  }

  function login($name, $pass) {
    $param[] = new SOAP_Value('name', '', $name, array());
    $param[] = new SOAP_Value('password', '', $pass, array());

    $res = $this->client->call('AuthRequest', $param, $this->options);
    if (PEAR::isError($res)) {
      watchdog('zimbra', 'Login to the zimbra server %url failed with the error: %error.', array('%url' => $this->url, '%error' => $res->getMessage()), WATCHDOG_ERROR);
      return FALSE;
    }
    else {
      $this->header = new SOAP_Header($this->context, '', array('authToken' => $res['authToken'], 'sessionId' => $res['sessionId'], 'nonotify' => '', 'noquality' => ''));
      return $res;
    }
  }

  function execute($command, $param = array()) {
    $params = array();
    foreach ($param as $k => $v) {
      $params[] = new SOAP_Value($k, '', $v['value'], isset($v['attr']) ? $v['attr'] : NULL);
    }
    $this->client->addHeader($this->header);
    $res = $this->client->call('{'. $this->namespace .'}'. $command, $params, $this->options);
    if (PEAR::isError($res)) {
      $notice_strings = _zimbra_notice_strings();
      $severity = (array_key_exists($command, $notice_strings) && preg_match($notice_strings[$command], $res->getMessage())) ? WATCHDOG_NOTICE : WATCHDOG_ERROR;
      watchdog('zimbra', 'Execution of the command %command on the zimbra server %url failed with the error: %error.', array('%command' => $command, '%url' => $this->url, '%error' => $res->getMessage()), $severity);
      return $severity == WATCHDOG_ERROR ? FALSE : 0;;
    }
    else {
      return $res ? $res : TRUE;
    }
  }
}


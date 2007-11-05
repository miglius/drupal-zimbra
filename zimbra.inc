<?php

require_once('SOAP/Client.php');

class ZimbraSoapClient
{
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
    $this->options = array('namespace' => $this->namespace, 'trace' => 1); 

    $this->client = new SOAP_Client($this->url);
    if (preg_match('/^https/', $url)) {
      $this->client->setOpt('curl', CURLOPT_SSL_VERIFYPEER, false);
    }
    $this->client->setTrace(true);
  }

  function login($name, $pass) {
    $param[] = new SOAP_Value('name', '', $name, array());
    $param[] = new SOAP_Value('password', '', $pass, array());
    
    $res = $this->client->call('AuthRequest', $param, $this->options);
    if (!is_object($res)) {
      $this->header = new SOAP_Header($this->context, '', array('authToken' => $res['authToken'], 'sessionId' => $res['sessionId'], 'nonotify' => '', 'noquality' => ''));
    }  
    return $res;
  }

  function execute($command, $params = array()) {
    $param = array();
    foreach ($params as $k => $v) {
      $param[] = new SOAP_Value($k, '', $v['value'], $v['attr']);
    }
    $this->client->addHeader($this->header);
    $res = $this->client->call('{'. $this->namespace .'}'. $command, $param, $this->options);
    return $res;
  }
}

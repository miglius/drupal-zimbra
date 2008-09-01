<?php
// $Id$
/**
 * @file
 * Module admin page callbacks.
 */

//////////////////////////////////////////////////////////////////////////////
// Zimbra settings

/**
 * Implements the settings page.
 *
 * @return
 *   The form structure.
 */
function zimbra_admin_settings() {
  $form['zimbra'] = array(
    '#type' => 'fieldset',
    '#title' => t('Zimbra settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE
  );
  $form['zimbra']['zimbra_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Zimbra e-mail account provisioning.'),
    '#default_value' => ZIMBRA_ENABLED,
  );
  $form['zimbra']['zimbra_soap_url'] = array(
    '#type' => 'textfield',
    '#title' => t('SOAP URL'),
    '#default_value' => ZIMBRA_SOAP_URL,
    '#size' => 50,
    '#maxlength' => 255,
    '#description' => t('Zimbra SOAP interface URL (like https://your.site.name:7071/service/admin/soap/).'),
  );
  $form['zimbra']['zimbra_domain'] = array(
    '#type' => 'textfield',
    '#title' => t('Domain'),
    '#default_value' => ZIMBRA_DOMAIN,
    '#size' => 50,
    '#maxlength' => 255,
    '#description' => t('The mail domain zimbra accounts will be created in.'),
  );
  $form['zimbra']['zimbra_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => ZIMBRA_NAME,
    '#size' => 30,
    '#maxlength' => 255,
    '#description' => t('Zimbra adminisration user name.'),
  );
  if (ZIMBRA_PASS_CLEAR || !ZIMBRA_PASS) {
    variable_del('zimbra_pass');
    $form['zimbra']['zimbra_pass'] = array(
      '#type' => 'password',
      '#title' => t('Password'),
      '#default_value' => '',
      '#size' => 30,
      '#maxlength' => 255,
      '#description' => t('Zimbra adminisration password.'),
    );
    variable_set('zimbra_pass_clear', FALSE);
  }
  else {
    $form['zimbra']['zimbra_pass_clear'] = array(
      '#type' => 'checkbox',
      '#title' => t('Clear current administration password.'),
      '#default_value' => FALSE,
    );
  }
  $form['zimbra']['zimbra_timeout'] = array(
    '#type' => 'textfield',
    '#title' => t('Timeout'),
    '#default_value' => ZIMBRA_TIMEOUT,
    '#size' => 4,
    '#maxlength' => 4,
    '#description' => t('Timeout for connection to Zimbra server in seconds. Zero means no timeout set.'),
  );
  $form['zimbra']['zimbra_user_pass'] = array(
    '#type' => 'checkbox',
    '#title' => t('User passwords.'),
    '#default_value' => ZIMBRA_USER_PASS,
    '#description' => t('Create and maintain users\'s drupal passwords in Zimbra.'),
  );

  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => t('Save configuration'),
  );
  $form['reset'] = array(
    '#type'  => 'submit',
    '#value' => t('Reset to defaults'),
  );

  return $form;
}

/**
 * Validate hook for the settings form.
 */
function zimbra_admin_settings_validate($form, &$form_state) {
  $values = $form_state['values'];
  $op = $form_state['clicked_button']['#value'];
  switch ($op) {
    case t('Save configuration'):
      if ($values['zimbra_enabled']) {
        if (empty($values['zimbra_soap_url']))
          form_set_error('zimbra_soap_url', t('A Zimbra SOAP interface URL cannot be empty.'));
        if (empty($values['zimbra_domain']))
          form_set_error('zimbra_domain', t('A mail domain cannot be empty.'));
        if (empty($values['zimbra_name']))
          form_set_error('zimbra_name', t('An adminisration user name cannot be empty.'));
        if (array_key_exists('zimbra_pass', $values) && empty($values['zimbra_pass']))
          form_set_error('zimbra_pass', t('An adminisration password cannot be empty.'));
        if (!is_numeric($values['zimbra_timeout']))
          form_set_error('zimbra_timeout', t('A timeout should be numeric.'));
      }
      break;
  }
}

/**
 * Submit hook for the settings form.
 */
function zimbra_admin_settings_submit($form, &$form_state) {
  $values = $form_state['values'];
  $op = $form_state['clicked_button']['#value'];
  switch ($op) {
    case t('Save configuration'):
      variable_set('zimbra_enabled', $values['zimbra_enabled']);
      variable_set('zimbra_soap_url', $values['zimbra_soap_url']);
      variable_set('zimbra_domain', $values['zimbra_domain']);
      variable_set('zimbra_name', $values['zimbra_name']);
      if (!empty($values['zimbra_pass'])) {
        variable_set('zimbra_pass', $values['zimbra_pass']);
      }
      if ($values['zimbra_pass_clear'] == 1) {
        variable_set('zimbra_pass', '');
      }
      variable_set('zimbra_timeout', $values['zimbra_timeout']);
      variable_set('zimbra_user_pass', $values['zimbra_user_pass']);
      drupal_set_message(t('The configuration options have been saved.'));
      break;
    case t('Reset to defaults'):
      variable_del('zimbra_enabled');
      variable_del('zimbra_soap_url');
      variable_del('zimbra_domain');
      variable_del('zimbra_name');
      variable_del('zimbra_pass');
      variable_del('zimbra_pass_clear');
      variable_del('zimbra_user_timeout');
      variable_del('zimbra_user_pass');
      drupal_set_message(t('The configuration options have been reset to their default values.'));
      break;
  }
}


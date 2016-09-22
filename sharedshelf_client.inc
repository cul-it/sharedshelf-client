<?php
/**
 * @file
 * Contains classes for Client connections handlers.
 */

/**
 * Base class for Drupal client connections.
 */
class clients_connection_shared_shelf extends clients_connection_base {
  // ============================================ Base: Constructor.
  /**
   * Constructor method.
   *
   * @param $object
   *  An object of class stdClass returned from CTools.
   */

  private $cookie_file = '/tmp/clients_connection_shared_shelf_cookie.txt';
  private $cookie_jar_file = '/tmp/clients_connection_shared_shelf_cookie.txt';

  function __construct($object) {
    // Call the base class to set the connection properties.
    parent::__construct($object);

    // Decrypt the password.
    if (!function_exists('clients_drupal_decrypt')) {
      module_load_include('module', 'clients', 'connections/clients_drupal/clients_drupal');
    }
    $this->configuration['password'] = clients_drupal_decrypt($this->configuration['password']);

  }

  // ============================================ Base: Testing system.
  /**
   * Provide buttons for the connection testing page.
   *
   * @param $form_state
   *  This is passed in so you can set defaults based on user input.
   */
  function getTestOperations($form_state, $connection_name) {
    $buttons['connect'] = array(
      '#value' => t('Test connection'),
      '#type' => 'submit',
      //'#name' => 'connect', // wtf does this do?
      '#action_type' => 'method',
      '#action_submit' => 'testConnectionConnect',
      '#description' => t('Test the connection settings by calling system.connect on the remote server.'),
    );
    $buttons['login'] = array(
      '#value' => t('Test user login'),
      '#type' => 'submit',
      //'#name' => 'login',
      '#action_type' => 'method',
      '#action_submit' => 'testConnectionLogin',
      '#description' => t('Test the remote user settings and by calling user.login on the remote server.'),
    );
    $buttons['logout'] = array(
      '#value' => t('Test user logout'),
      '#type' => 'submit',
      //'#name' => 'login',
      '#action_type' => 'method',
      '#action_submit' => 'testConnectionLogout',
      '#description' => t('Test the remote user settings and by calling user.logout on the remote server.'),
    );
    return $buttons;
  }

  /**
   * Connection test button handler: basic connection.
   *
   * Connection test handlers should return the raw data they got back from the
   * connection for display to the user.
   */
  function testConnectionConnect(&$button_form_values) {
    try {
      // Call the connect method.
      $connect = $this->callMethodArray('system.connect');
    }
    catch (Exception $e) {
      drupal_set_message(t('Could not connect to the remote site, got error message "@message".', array(
        '@message' => $e->getMessage(),
      )), 'warning');
      //dsm($e);

      return;
    }

    $response = json_decode($connect, TRUE);
    if (is_array($response) && isset($response['user_id'])) {
      drupal_set_message(t('Sucessfully connected to the remote site.'));
    }
    else {
      drupal_set_message(t('Could not connect to the remote site.'), 'warning');
    }

    return $connect;
  }

  /**
   * Connection test button handler: user login.
   */
  function testConnectionLogin(&$button_form_values) {
    try {
      // Call the login method.
      $login = $this->callMethodArray('user.login');
      // Eep. we need user details!!!
      //
    }
    catch (Exception $e) {
      drupal_set_message(t('Could not log in to the remote site, got error message "@message".', array(
        '@message' => $e->getMessage(),
      )), 'warning');
      //dsm($e);

      return;
    }

    return $login;
  }


  /**
   * Connection test button handler: user logout.
   */
  function testConnectionLogout(&$button_form_values) {
    try {
      // Call the login method.
      $login = $this->callMethodArray('user.logout');
      // Eep. we need user details!!!
    }
    catch (Exception $e) {
      drupal_set_message(t('Could not log out of the remote site, got error message "@message".', array(
        '@message' => $e->getMessage(),
      )), 'warning');
      //dsm($e);

      return;
    }
    return $login;
  }


  /**
   * Extra form elements specific to a class's edit form.
   *
   * This is the same pattern as node_form() -- just ignore the object behind
   * the curtain ;)
   *
   * @param $form_state
   *  The form state from the main form, which you probably don't need anyway.
   *
   * @return
   *  A FormAPI form array. This will be merged in with basic data and the
   *  submit button added.
   *
   * @see clients_connection_form()
   * @see clients_connection_form()
   * @see clients_connection_form_submit()
   */
  function connectionSettingsForm(&$form_state) {
    $form = array();

    $form['endpoint'] = array(
      '#type' => 'textfield',
      '#title' => t('Connection endpoint'),
      '#default_value' => $this->new ? '' : $this->endpoint,
      '#size' => 50,
      '#maxlength' => 100,
      '#description' => t('Remote service URL e.g. http://catalog.stage3.sharedshelf.artstor.org'),
      '#required' => TRUE,
    );

    $form['configuration'] = array(
      '#type' => 'fieldset',
      '#title' => t('Configuration'),
      '#collapsible' => FALSE,
      '#tree' => TRUE,
    );

    $form['configuration']['username'] = array(
      '#type' => 'textfield',
      '#title' => t('ARTstor email'),
      '#default_value' => $this->new ? '' : $this->configuration['username'],
      '#size' => 30,
      '#maxlength' => 60,
      '#attributes' => array('autocomplete' => 'off'),
      '#description' => t('This should be same as your account\'s email on the server you are connecting to.'),
      '#required' => TRUE,
    );

    $password_desc = $this->new
      ? t('This should be same as the password on the server you are connecting to.')
      : t('This should be same as the password on the server you are connecting to. Leave blank unless you need to change this.');
    $form['configuration']['password'] = array(
      '#type' => 'password',
      '#title' => t('ARTstor password'),
      '#size' => 30,
      '#maxlength' => 60,
      '#attributes' => array('autocomplete' => 'off'),
      '#description' => $password_desc,
      '#required' => $this->new,
    );

    return $form;
  }

  /**
   * Submit handler for saving/updating connections of this class.
   *
   * @see clients_connection_form_submit()
   */
  function connectionSettingsForm_submit($form, &$form_state) {
    $old_connection = $form_state['values']['old_connection'];

    // Check whether we're editing or adding a new connection.
    if ($old_connection->new) {
      //jgrtest $form_state['values']['configuration']['password'] = clients_drupal_encrypt($form_state['values']['configuration']['password']);
      $form_state['values']['configuration']['password'] = $form_state['values']['configuration']['password'];
    }
    else {
      // Prepare password for serialized storage
      if (empty($form_state['values']['configuration']['password'])) {
        // Set password to original if blank.
        $form_state['values']['configuration']['password'] = $old_connection->configuration['password'];
      }
      //jgrtest $form_state['values']['configuration']['password'] = clients_drupal_encrypt($form_state['values']['configuration']['password']);
      $form_state['values']['configuration']['password'] = $form_state['values']['configuration']['password'];
    }
  }

  function formatEndpoint($url) {
    return $url;
  }

  /**
   * Call a remote method with an array of parameters.
   *
   * This is technically internal; use the more DX-friendly callMethod() or
   * the all-in-one clients_connection_call().
   *
   * @param $method
   *  The name of the remote method to call.
   * @param
   *  All other parameters are passed to the remote method.
   *
   * @return
   *  Whatever is returned from the remote site.
   */
  function callMethodArray($method, $method_params = array()) {
    // If HTTP requests are enabled, report the error and do nothing.
    // (Cribbed from Content distribution module.)
    if (variable_get('drupal_http_request_fails', FALSE) == TRUE) {
      drupal_set_message(t('Drupal is unable to make HTTP requests. Please reset the HTTP request status.'), 'error', FALSE);
      watchdog('clients', 'Drupal is unable to make HTTP requests. Please reset the HTTP request status.', array(), WATCHDOG_CRITICAL);
      return;
    }

    $params = array();

    switch ($method) {
      case 'user.login':
        // Login and creation session
        $params['email'] = $this->configuration['username'];
        $params['password'] = $this->configuration['password'];
        $result = $this->post('account', $params);
        break;
      case 'system.connect':
        // Returns the user emails and id and login status.
        //$params['_dc'] = '1342619234905'; // ?? wherever this comes from
        $result = $this->get('account');
        break;
      case 'user.logout':
        // User logs out
        $result = $this->delete('account');
        break;
      case 'projects':
        // Returns list of projects accessible by the user.
        // TODO: where does _dc come from?
        //$params['_dc'] = '1342619234905'; // ?? wherever this comes from
        $result = $this->get('projects', $params);
        break;
      case 'projects.assets':
        // Returns list of projects accessible by the user.
        // expects $method_params = array( {project_id} )
        $project_id = $method_params[0];
        //$params['_dc'] = '1342619234905'; // ?? wherever this comes from
        // TODO: deal with paging when > 25 assets
        $result = $this->get("projects/$project_id/assets", $params);
        break;
      case 'asset.full':
        // Returns URL for full sized image
        // expects $method_params = array( {asset id - (ssid)} )
        $asset_id = $method_params[1];
        //$params['_dc'] = '1342619234905'; // ?? wherever this comes from
        $result = $this->get("assets/$asset_id/representation", $params);
        break;
      case 'asset.thumbnail':
        // Returns URL for reduced sized image
        // expects $method_params = array( {asset id - (ssid)}, {size id - ( one of 64, 128, 256, 512, 1024 )} )
        $asset_id = $method_params[1];
        $size = $method_params[2];
        if (!in_array($size, array(64, 128, 256, 512, 1024))) {
          throw new Exception("Invalid thumbnail size: $size. Must be one of 64,128,256,512,1024");
        }
        //$params['_dc'] = '1342619234905'; // ?? wherever this comes from
        $result = $this->get("assets/$asset_id/representation/$size", $params);
        break;

      default :
        dsm('strange method for callMethodArray: ' . $method);
        break;
    }

    return $result;
  }

  private function post($path = 'account', $params = array()) {
    // $path is part of endpoint url after .../artstor.org/
    // $params is key => value array
    $postfields = http_build_query($params);
    $url = $this->endpoint . "/$path";
    $ch = curl_init($url);

    curl_setopt($ch, CURLOPT_POST, 1);
    curl_setopt($ch, CURLOPT_POSTFIELDS, $postfields);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
    // TODO: save cookies in file?
    curl_setopt($ch, CURLOPT_COOKIEFILE, $this->cookie_file);
    curl_setopt($ch, CURLOPT_COOKIEJAR, $this->cookie_jar_file);

    $response = curl_exec($ch);
    curl_close($ch);
    // $response = json_decode($response, TRUE);
    return $response;
  }

  public function delete($path = 'account', $params = array()) {
    // $path is part of endpoint url after .../artstor.org/
    // $params is key => value array
    $postfields = http_build_query($params);
    $url = sprintf($this->endpoint . "/$path");
    $ch = curl_init($url);

    curl_setopt($ch, CURLOPT_POSTFIELDS, $postfields);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
    curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
    curl_setopt($ch, CURLOPT_HEADER, 0);
    curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_setopt($ch, CURLOPT_COOKIEFILE, $this->cookie_file);
    curl_setopt($ch, CURLOPT_COOKIEJAR, $this->cookie_jar_file);

    $response = curl_exec($ch);
    curl_close($ch);
    // $response = json_decode($response, TRUE);
    return $response;
  }

  private function get($path = 'account', $params = array()) {
    // $path is part of endpoint url after .../artstor.org/
    // $params is key => value array
    $postfields = http_build_query($params);
    $url = $this->endpoint . "/$path?$getfields";
    dsm($url);
    $ch = curl_init($url);

    curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
    // TODO: save cookies in file?
    curl_setopt($ch, CURLOPT_COOKIEFILE, $this->cookie_file);
    curl_setopt($ch, CURLOPT_COOKIEJAR, $this->cookie_jar_file);

    $response = curl_exec($ch);
    curl_close($ch);
    // $response = json_decode($response, TRUE);
    return $response;
  }
}

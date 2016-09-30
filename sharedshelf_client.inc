<?php
/**
 * @file
 * Contains classes for Client connections handlers.
 */

/**
 * Base class for Drupal client connections.
 */
class clients_connection_sharedshelf_client extends clients_connection_base {
  // ============================================ Base: Constructor.
  /**
   * Constructor method.
   *
   * @param $object
   *  An object of class stdClass returned from CTools.
   */

  private $cookie_file = '/tmp/clients_connection_sharedshelf_client_cookie.txt';
  private $cookie_jar_file = '/tmp/clients_connection_sharedshelf_client_cookie.txt';

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
    $buttons['reset'] = array(
      '#value' => t('Reset the HTTP request status'),
      '#type' => 'submit',
      //'#name' => 'login',
      '#action_type' => 'method',
      '#action_submit' => 'testConnectionResetHttpRequest',
      '#description' => t('Reset the HTTP request status so Drupal can make HTTP requests.'),
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
      //dpm($e);

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
      //dpm($e);

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
      //dpm($e);

      return;
    }
    return $login;
  }

  /**
   * Connection test button handler: reset the HTTP request status.
   */
  function testConnectionResetHttpRequest(&$button_form_values) {
    try {
      // Handle the situation where this message shows up:
      //  Drupal is unable to make HTTP requests. Please reset the HTTP request status.
      //  see https://www.drupal.org/node/1186572
      variable_del('drupal_http_request_fails');
      $login = '';
    }
    catch (Exception $e) {
      drupal_set_message(t('Could not reset the HTTP request status, got error message "@message".', array(
        '@message' => $e->getMessage(),
      )), 'warning');
      //dpm($e);

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
  function connectionSettingsFormAlter(&$form, &$form_state) {

    // add validate to encrypt password
    $form['#validate'][] = 'sharedshelf_client_connection_settings_form_validate';

    $is_new = isset($this->is_new) && $this->is_new;

    $form['endpoint'] = array(
      '#type' => 'textfield',
      '#title' => t('Connection endpoint'),
      '#default_value' => $is_new ? '' : $this->endpoint,
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
      '#default_value' => $is_new ? '' : $this->configuration['username'],
      '#size' => 30,
      '#maxlength' => 60,
      '#attributes' => array('autocomplete' => 'off'),
      '#description' => t('This should be same as your account\'s email on the server you are connecting to.'),
      '#required' => TRUE,
    );

    $password_desc = $is_new
      ? t('This should be same as the password on the server you are connecting to.')
      : t('This should be same as the password on the server you are connecting to. Leave blank unless you need to change this.');
    $form['configuration']['password'] = array(
      '#type' => 'password',
      '#title' => t('ARTstor password'),
      '#size' => 30,
      '#maxlength' => 60,
      '#attributes' => array('autocomplete' => 'off'),
      '#description' => $password_desc,
      '#required' => TRUE,
    );
  }

  /**
   * Submit handler for saving/updating connections of this class.
   *
   * @see clients_connection_form_submit()
   */
  function connectionSettingsForm_submit($form, &$form_state) {
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
    try {
      // If HTTP requests are enabled, report the error and do nothing.
      // (Cribbed from Content distribution module.)
      if (variable_get('drupal_http_request_fails', FALSE) == TRUE) {
        drupal_set_message(t('Drupal is unable to make HTTP requests. Please reset the HTTP request status.'), 'error', FALSE);
        watchdog('clients', 'Drupal is unable to make HTTP requests. Please reset the HTTP request status.', array(), WATCHDOG_CRITICAL);
        throw new Exception("Drupal is unable to make HTTP requests.", 1);
      }

      $params = array();
      $result = '';

      switch ($method) {
        case 'user.login':
          if (module_exists('encrypt') && function_exists('decrypt')) {
              $password = decrypt($this->configuration['password']);
          }
          else {
            throw new Exception("Unable to decrypt the password.", 1);
          }
          // Login and creation session
          $params['email'] = $this->configuration['username'];
          $params['password'] = $password;
          $result = $this->post('account', $params);
          break;
        case 'system.connect':
          // Returns the user emails and id and login status.
          $result = $this->get('account');
          break;
        case 'user.logout':
          // User logs out
          $result = $this->delete('account');
          break;
        case 'projects':
          // Returns list of projects accessible by the user.
          $result = $this->get('projects', $params);
          break;
        case 'projects.assets':
          // Returns list of assets accessible by the user.
          // expects $method_params = array( {project_id} )
          $project_id = $method_params[0];
          // find out the total asset count
          $params = array(
            'start' => 0,
            'limit' => 1,
            'with_meta' => FALSE,
            );
          $result = $this->get("projects/$project_id/assets", $params);
          $result = json_decode($result, TRUE);
          $total = $result['total'];

          $params = array(
            'start' => 0,
            'limit' => $total,
            'with_meta' => FALSE,
            'sort' => 'id',
            'dir' => 'ASC'
            );
          // TODO: deal with paging when > 25 assets
          $result = $this->get("projects/$project_id/assets", $params);
          break;
        case 'asset.full':
          // Returns URL for full sized image
          // expects $method_params = array( {asset id - (ssid)} )
          $asset_id = $method_params[1];
          $result = $this->get_url("assets/$asset_id/representation", $params);
          break;
        case 'asset.thumbnail':
          // Returns URL for reduced sized image
          // expects $method_params = array( {asset id - (ssid)}, {size id - ( [0-4] )} )
          // sizes: 0=64, 1=128,2=256, 3=512, 4=1024
          $asset_id = $method_params[1];
          $size = $method_params[2];
          if ($size < 0 || $size > 4) {
            throw new Exception("Invalid thumbnail size: $size. Must be one of 0,1,2,3,4");
          }
          $result = $this->get_url("assets/$asset_id/representation/$size", $params);
          break;
        case 'feeds.assets':
          // construct a list of assets suitable for feeds
          // expects $method_params = array( {project_id} )
          $project_id = $method_params[0];
          $result = $this->get_assets($project_id);
          break;

        default :
          dpm('strange method for callMethodArray: ' . $method);
          break;
      }

      return $result;
    }
    catch (Exception $e) {
      form_set_error('Client methods', $e->getMessage());
      $result = FALSE;
    }

    return $result;
  }

  private function get_assets($project_id) {
    try {
      // be sure user is logged in
      $login = $this->callMethodArray('user.login');

      $result = 'get_assets';

      // log user out
      $logout = $this->callMethodArray('user.logout');
    }
    catch (Exception $e) {
      form_set_error('get_assets', $e->getMessage());
      $result = FALSE;
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
    return $response;
  }

  private function get($path = 'account', $params = array()) {
    // $path is part of endpoint url after .../artstor.org/
    // $params is key => value array
    $getfields = http_build_query($params);
    $url = $this->endpoint . "/$path?$getfields";
    //dpm($url);
    $ch = curl_init($url);

    curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
    // TODO: save cookies in file?
    curl_setopt($ch, CURLOPT_COOKIEFILE, $this->cookie_file);
    curl_setopt($ch, CURLOPT_COOKIEJAR, $this->cookie_jar_file);

    $response = curl_exec($ch);
    curl_close($ch);
    return $response;
  }

  private function get_url($path = '/account', $params = array(), $require_extension = TRUE) {
    // sometimes the first time gets the url without an extension
    $getfields = http_build_query($params);
    $url = $this->endpoint . "/$path?$getfields";
    $ch = curl_init($url);
    if ($ch === FALSE) {
      curl_close($ch);
      throw new Exception("Bad request url in get_url: $url", 1);
    }
    $options = array(
      CURLOPT_CONNECTTIMEOUT => 120,
      CURLOPT_COOKIEFILE =>  $this->cookie_file, /* make sure you provide FULL PATH to cookie files*/
      CURLOPT_COOKIEJAR => $this->cookie_jar_file,
      CURLOPT_FOLLOWLOCATION => FALSE,  // We want to just get redirect url but not to follow it.
      CURLOPT_HEADER => TRUE,    // We'll parse redirect url from header.
      CURLOPT_RETURNTRANSFER => TRUE,
      CURLOPT_TIMEOUT => 120,
      );
    $allswell = curl_setopt_array($ch, $options);
    if ($allswell === FALSE) {
      throw new Exception("get_url failed to set curl options", 1);
    }
    $response = curl_exec($ch);
    preg_match_all('/^Location:(.*)$/mi', $response, $matches);
    $url2 = !empty($matches[1]) ? trim($matches[1][0]) : 'No redirect found';
    curl_close($ch);

    $extension = pathinfo($url2, PATHINFO_EXTENSION);
    if (!empty($extension) || $require_extension === FALSE) {
      return $url2;
    }

    // $url2 comes back without an extension (eg. .jpg) so get second redirect
    $ch = curl_init($url2);
    if ($ch === FALSE) {
      curl_close($ch);
      throw new Exception("Bad second request url: $url2", 1);
    }
    // re-use curl options
    $allswell = curl_setopt_array($ch, $options);
    if ($allswell === FALSE) {
      throw new Exception("get_url failed to set curl options 2", 1);
    }
    $response = curl_exec($ch);
    curl_close($ch);
    preg_match_all('/^Location:(.*)$/mi', $response, $matches);
    if (empty($matches[1])) {
      throw new Exception("Can't find url for $url", 1);
    }

    $url_out = trim($matches[1][0]);
    return $url_out;
  }

}

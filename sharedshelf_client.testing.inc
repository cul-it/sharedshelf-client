<?php
/**
 * @file
 * Contains classes for Client connection testing.
 */

/**
 * Test basic connection to a Drupal Services connection.
 */
class ClientsConnectionSSTestConnect implements ClientsConnectionTestingInterface {

  /**
   * The labels for the test.
   *
   * (This is because it would seem you can't define class variables using
   * expressions.)
   */
  function testLabels() {
    return array(
      'label' => t('Test connection'),
      'description' => t('Test the basic connection to the site by calling system.connect.'),
      'button' => t('Connect'),
    );
  }

  /**
   * Execute the test.
   *
   * Connection test handlers should return the raw data they got back from the
   * connection for display to the user.
   *
   * @param $connection
   *  The connection handler.
   * @param $button_form_values
   *  The form values for the test form element. The values for elements added
   *  to the form are in $button_form_values['params'].
   *
   * @return
   *  Data from the remote connection. This is output to the form as raw data.
   */
  function test($connection, &$button_form_values) {
    try {
      // Call the connect method.
      $connect = $connection->callMethodArray('system.connect');
      $connect = json_decode($connect);

      if (is_array($connect) && isset($connect['logged_in']) && $connect['logged_in'] === true) {
        drupal_set_message(t('Sucessfully connected to the remote site.'));
      }
      else {
        drupal_set_message(t('Could not connect to the remote site.'), 'warning');
      }
   }
   catch (Exception $e) {
      drupal_set_message(t('Could not connect to the remote site, got error message "@message".', array(
        '@message' => $e->getMessage(),
      )), 'warning');
      $connect = FALSE;
   }

    return $connect;
  }

}


/**
 * Test login to a Drupal Services connection.
 */
class ClientsConnectionSSTestLogin implements ClientsConnectionTestingInterface {

  /**
   * The labels for the test.
   */
  function testLabels() {
    return array(
      'label' => t('Test login'),
      'description' => t('Test the basic connection to the site by calling user.login.'),
      'button' => t('Log in'),
    );
  }

  /**
   * Connection test button handler: user login.
   */
  function test($connection, &$button_form_values) {
    try {
      // Call the login method.
      $login = $connection->callMethodArray('user.login');
      $login = json_decode($login);

      if (is_array($login) && isset($login['success']) && $login['success'] === true) {
        drupal_set_message(t('Sucessfully logged in to the remote site; got back details for user %user (profile @uid).', array(
          '%user' => $login['username'],
          '@uid'  => $login['profile_id'],
          )));
      }
      else {
        drupal_set_message(t('Could not log in to the remote site.'), 'warning');
      }
    }
    catch (Exception $e) {
      drupal_set_message(t('Could not log in to the remote site, got error message "@message".', array(
        '@message' => $e->getMessage(),
      )), 'warning');
      //dsm($e);

      $login = FALSE;
    }

    return $login;
  }

}


/**
 * Test login to a Drupal Services connection.
 */
class ClientsConnectionSSTestLogout implements ClientsConnectionTestingInterface {

  /**
   * The labels for the test.
   */
  function testLabels() {
    return array(
      'label' => t('Test logout'),
      'description' => t('Test the logout from the site by calling user.logout.'),
      'button' => t('Log out'),
    );
  }

  /**
   * Connection test button handler: basic connection.
   */
  function test($connection, &$button_form_values) {
    try {
      // Call the login method.
      $logout = $connection->callMethodArray('user.logout');
      $logout = json_decode($logout);
      if (is_array($logout) && isset($logout['success']) && $logout['success'] === true) {
        drupal_set_message(t('Sucessfully logged out of the remote site.'));
      }
      else {
        drupal_set_message(t('Could not log out of the remote site.'), 'warning');
      }
    }
    catch (Exception $e) {
      drupal_set_message(t('Could not log out of the remote site, got error message "@message".', array(
        '@message' => $e->getMessage(),
      )), 'warning');
      //dsm($e);

      $logout = FALSE;
    }


    return $logout;
  }

}


/**
 * Test http requests reset.
 */
class ClientsConnectionSSTestReset implements ClientsConnectionTestingInterface {

  /**
   * The labels for the test.
   */
  function testLabels() {
    return array(
      'label' => t('Reset the HTTP request status'),
      'description' => t('Handle this message: Drupal is unable to make HTTP requests. Please reset the HTTP request status.'),
      'button' => t('Reset'),
    );
  }

  /**
   * Connection test button handler: basic connection.
   */
  function test($connection, &$button_form_values) {
    try {
      variable_del('drupal_http_request_fails');
      $logout = '';
    }
    catch (Exception $e) {
      drupal_set_message(t('Could not reset the HTTP request status, got error message "@message".', array(
        '@message' => $e->getMessage(),
      )), 'warning');
      //dsm($e);

      $logout = FALSE;
    }
    return $logout;
  }

}

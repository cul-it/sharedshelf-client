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
    }
    catch (Exception $e) {
      drupal_set_message(t('Could not connect to the remote site, got error message "@message".', array(
        '@message' => $e->getMessage(),
      )), 'warning');
      //dsm($e);

      return;
    }

    if (is_array($connect) && isset($connect['user'])) {
      drupal_set_message(t('Sucessfully connected to the remote site.'));
    }
    else {
      drupal_set_message(t('Could not connect to the remote site.'), 'warning');
    }

    return $connect;
  }

}
<?php

/**
 * @file
 * Retrieve the access rights value from an object, using caching.
 */

function ubl_access_rights_from_metadata_get_access_rights_value($obj, $config, $rebuildcache = FALSE) {
  $id2acvalue = &drupal_static(__FUNCTION__, array());
  $id = $obj->id;
  if ($rebuildcache) {
    unset($id2acvalue[$id]);
  }
  if (!array_key_exists($id, $id2acvalue)) {
    $cachename = $id;
    $cachebin = 'cache_ubl_access_rights_from_metadata_access_rights_value';
    if (!$rebuildcache) {
      if ($cache = cache_get($cachename, $cachebin)) {
        $id2acvalue[$id] = $cache->data;
      }
    }
    if (!array_key_exists($id, $id2acvalue)) {
      module_load_include('inc', 'ubl_access_rights_from_metadata', 'includes/config');
      $rightssourcesettings = ubl_access_rights_from_metadata_get_rights_source_settings($config);
      $dsid = $rightssourcesettings['dsid'];
      if (isset($obj[$dsid])) {
        $xpaths = $rightssourcesettings['xpath'];
        if (!is_array($xpaths)) {
          $xpaths = array(0 => $xpaths);
        }
        $content = $obj[$dsid]->content;
        $domdoc = new DOMDocument();
        if ($domdoc->loadXML($content)) {
          $id2acvalue[$id] = ''; // subtle change to indicate that the DS XML exists, but does not have any access rights info
          $domxpath = new DOMXPath($domdoc);
          foreach ($rightssourcesettings['namespace'] as $ns) {
            list($prefix, $uri) = explode(' ', $ns, 2);
            $domxpath->registerNamespace($prefix, $uri);
          }
          foreach ($xpaths as $xpath) {
            $domnodelist = $domxpath->query($xpath);
            if ($domnodelist && $domnodelist->length > 0) {
              foreach ($domnodelist as $domnode) {
                $text = $domnode->textContent;
                if (isset($text) && strlen($text) > 0) {
                  if (strlen($id2acvalue[$id]) > 0) {
                    $id2acvalue[$id] .= '--';
                  }
                  $id2acvalue[$id] .= $text;
                }
              }
            }
          }
        }
      }
      if (!array_key_exists($id, $id2acvalue)) {
        $id2acvalue[$id] = NULL;
      }
      cache_set($cachename, $id2acvalue[$id], $cachebin);
    }
  }
  return $id2acvalue[$id];
}
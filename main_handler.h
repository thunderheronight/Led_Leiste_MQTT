void srv_handle_not_found() {
  server.send(404, "text/plain", "File Not Found");
}

void srv_handle_index_html() {
  if (e_mode == 200) {
    server.send_P(200, "text/html", index2_html);
  } else {
    server.send_P(200, "text/html", index_html);
  }
}

void srv_handle_main_js() {
  server.send_P(200, "application/javascript", main_js);
}

void srv_handle_check() {
  String mode_temp = ws2812fx.getModeName(e_mode);
  if (e_mode == 200) {
    server.send(200, "text/plain", "Music Visu");
  } else if (ws2812fx.isRunning() == 0) {
    server.send(200, "text/plain", "Aus");
  } else {
    Serial.print("Check: ");
    Serial.println(e_mode);
    server.send(200, "text/plain", ws2812fx.getModeName(e_mode));
  }
}

void srv_handle_increase_speed() {
  ws2812fx.increaseSpeed(10);
  client.publish("/led_leiste/get_speed", String(ws2812fx.getSpeed()).c_str(), true);
  server.send(200, "text/plain", ws2812fx.getModeName(e_mode));
}

void srv_handle_decrease_speed() {
  ws2812fx.decreaseSpeed(10);
  client.publish("/led_leiste/get_speed", String(ws2812fx.getSpeed()).c_str(), true);
  server.send(200, "text/plain", ws2812fx.getModeName(e_mode));
}

void srv_handle_increase_bright() {
  ws2812fx.increaseBrightness(10);
  client.publish("/led_leiste/get_bright", String(ws2812fx.getBrightness()).c_str(), true);
  server.send(200, "text/plain", ws2812fx.getModeName(e_mode));
}

void srv_handle_decrease_bright() {
  ws2812fx.decreaseBrightness(10);
  client.publish("/led_leiste/get_bright", String(ws2812fx.getBrightness()).c_str(), true);
  server.send(200, "text/plain", ws2812fx.getModeName(e_mode));
}

void srv_handle_save() {
  eeprom_change(e_mode);
  server.send(200, "text/plain", "Modus dauerhaft gespeichert");
}

void srv_handle_set_color() {
  uint32_t color_tmp = (uint32_t) strtol(&server.arg("color_hex")[0], NULL, 16);
  if (color_tmp >= 0x000000 && color_tmp <= 0xFFFFFF) {
    ws2812fx.setColor(color_tmp);
    Serial.println(ws2812fx.getColor());
    client.publish("/led_leiste/get_color", String(color_tmp).c_str(), true);
  }
  server.send(200, "Text/plain", "Color changed");
}
void srv_handle_get_color() {
  String color_str = String(ws2812fx.getColor(), HEX);
  server.send(200, "Text/plain", color_str);
}

void srv_handle_off_on() {
  if (e_mode == 200) {
    e_mode = 201;
    server.send(200, "text/plain", "Aus");
  } else if (ws2812fx.isRunning() == 0) {
    ws2812fx.start();
    server.send(200, "text/plain", ws2812fx.getModeName(e_mode));
  } else {
    ws2812fx.stop();
    server.send(200, "text/plain", "Aus");
  }
}

//If event occurs
void event_f(int foll_temp) {
  event = foll_temp;
  Serial.println("Event!");
  if (e_mode == 200) {
    eeprom_change(ws2812fx.getModeCount() + event);
  }
  server.send(200, "text/plain", "Event!");
}

void M_CHANGE(int c_mode) {
  if (e_mode == 200) {
    client.publish("/led_leiste/get_mode", "Music Visu", true);
    ws2812fx.stop();
    delay(10);
    eeprom_change(c_mode + 100);
  }
  e_mode = c_mode;

  client.publish("/led_leiste/get_mode", String(ws2812fx.getModeName(e_mode)).c_str(), true);
  server.send(200, "text/plain", ws2812fx.getModeName(e_mode));
  Serial.println("New Mode: ");
  Serial.print(e_mode);
}

void http_server_handler() {
  Serial.println("HTTP server setup");
  server.on("/", srv_handle_index_html);
  server.on("/main.js", srv_handle_main_js);
  server.on("/modes", srv_handle_modes);
  server.on("/set", srv_handle_set);
  server.on("/check", srv_handle_check);
  server.on("/save", srv_handle_save);
  server.on("/set_color", srv_handle_set_color);
  server.on("/get_color", srv_handle_get_color);
  server.on("/off_on", srv_handle_off_on);
  server.on("/increase_speed", srv_handle_increase_speed);
  server.on("/decrease_speed", srv_handle_decrease_speed);
  server.on("/increase_brightness", srv_handle_increase_bright);
  server.on("/decrease_brightness", srv_handle_decrease_bright);
  //Event inc
  server.on("/event1", []() {
    event_f(1);
  });
  server.on("/event2", []() {
    event_f(2);
  });
  server.on("/event3", []() {
    event_f(3);
  });
  server.on("/event4", []() {
    event_f(4);
  });

  server.on("/mode_change0", []() {
    M_CHANGE(0);
  });
  server.on("/mode_change1", []() {
    M_CHANGE(1);
  });
  server.on("/mode_change2", []() {
    M_CHANGE(2);
  });
  server.on("/mode_change3", []() {
    M_CHANGE(3);
  });
  server.on("/mode_change4", []() {
    M_CHANGE(4);
  });
  server.on("/mode_change5", []() {
    M_CHANGE(5);
  });
  server.on("/mode_change6", []() {
    M_CHANGE(6);
  });
  server.on("/mode_change7", []() {
    M_CHANGE(7);
  });
  server.on("/mode_change8", []() {
    M_CHANGE(8);
  });
  server.on("/mode_change9", []() {
    M_CHANGE(9);
  });
  server.on("/mode_change10", []() {
    M_CHANGE(10);
  });
  server.on("/mode_change11", []() {
    M_CHANGE(11);
  });
  server.on("/mode_change12", []() {
    M_CHANGE(12);
  });
  server.on("/mode_change13", []() {
    M_CHANGE(13);
  });
  server.on("/mode_change14", []() {
    M_CHANGE(14);
  });
  server.on("/mode_change15", []() {
    M_CHANGE(15);
  });
  server.on("/mode_change16", []() {
    M_CHANGE(16);
  });
  server.on("/mode_change17", []() {
    M_CHANGE(17);
  });
  server.on("/mode_change18", []() {
    M_CHANGE(18);
  });
  server.on("/mode_change19", []() {
    M_CHANGE(19);
  });
  server.on("/mode_change20", []() {
    M_CHANGE(20);
  });
  server.on("/mode_change21", []() {
    M_CHANGE(21);
  });
  server.on("/mode_change22", []() {
    M_CHANGE(22);
  });
  server.on("/mode_change23", []() {
    M_CHANGE(23);
  });
  server.on("/mode_change24", []() {
    M_CHANGE(24);
  });
  server.on("/mode_change25", []() {
    M_CHANGE(25);
  });
  server.on("/mode_change26", []() {
    M_CHANGE(26);
  });
  server.on("/mode_change27", []() {
    M_CHANGE(27);
  });
  server.on("/mode_change28", []() {
    M_CHANGE(28);
  });
  server.on("/mode_change29", []() {
    M_CHANGE(29);
  });
  server.on("/mode_change30", []() {
    M_CHANGE(30);
  });
  server.on("/mode_change31", []() {
    M_CHANGE(31);
  });
  server.on("/mode_change32", []() {
    M_CHANGE(32);
  });
  server.on("/mode_change33", []() {
    M_CHANGE(33);
  });
  server.on("/mode_change34", []() {
    M_CHANGE(34);
  });
  server.on("/mode_change35", []() {
    M_CHANGE(35);
  });
  server.on("/mode_change36", []() {
    M_CHANGE(36);
  });
  server.on("/mode_change37", []() {
    M_CHANGE(37);
  });
  server.on("/mode_change38", []() {
    M_CHANGE(38);
  });
  server.on("/mode_change39", []() {
    M_CHANGE(39);
  });
  server.on("/mode_change40", []() {
    M_CHANGE(40);
  });
  server.on("/mode_change41", []() {
    M_CHANGE(41);
  });
  server.on("/mode_change42", []() {
    M_CHANGE(42);
  });
  server.on("/mode_change43", []() {
    M_CHANGE(43);
  });
  server.on("/mode_change44", []() {
    M_CHANGE(44);
  });
  server.on("/mode_change45", []() {
    M_CHANGE(45);
  });
  server.on("/mode_change46", []() {
    M_CHANGE(46);
  });
  server.on("/mode_change47", []() {
    M_CHANGE(47);
  });
  server.on("/mode_change48", []() {
    M_CHANGE(48);
  });
  server.on("/mode_change49", []() {
    M_CHANGE(49);
  });
  server.on("/mode_change50", []() {
    M_CHANGE(50);
  });

  server.on("/mode_change51", []() {
    M_CHANGE(51);
  });

  server.on("/mode_change52", []() {
    M_CHANGE(52);
  });

  server.on("/mode_change53", []() {
    M_CHANGE(53);
  });

  server.on("/mode_change54", []() {
    M_CHANGE(54);
  });

  server.on("/music_visu", []() {
    if (e_mode == 200) {
      server.send(200, "text/plain", "Läuft schon");
    } else {
      client.publish("/led_leiste/get_mode", "Music Visu");
      eeprom_change(200);
    }
  });

  //
  server.onNotFound(srv_handle_not_found);
  server.begin();
  Serial.println("HTTP server started.");
}

int get_modenum(String str_mode) {
  if (str_mode == "Static") {
    return (0);
  } else if (str_mode == "Blink") {
    return (1);
  } else if (str_mode == "Breath") {
    return (2);
  } else if (str_mode == "Color Wipe") {
    return (3);
  } else if (str_mode == "Color Wipe Random") {
    return (4);
  } else if (str_mode == "Random Color") {
    return (5);
  } else if (str_mode == "Single Dynamic") {
    return (6);
  } else if (str_mode == "Multi Dynamic") {
    return (7);
  } else if (str_mode == "Rainbow") {
    return (8);
  } else if (str_mode == "Rainbow Cycle") {
    return (9);
  } else if (str_mode == "Scan") {
    return (10);
  } else if (str_mode == "Dual Scan") {
    return (11);
  } else if (str_mode == "Fade") {
    return (12);
  } else if (str_mode == "Theater Chase") {
    return (13);
  } else if (str_mode == "Theater Chase Rainbow") {
    return (14);
  } else if (str_mode == "Running Lights") {
    return (15);
  } else if (str_mode == "Twinkle") {
    return (16);
  } else if (str_mode == "Twinkle Random") {
    return (17);
  } else if  (str_mode == "Twinkle Fade") {
    return (18);
  } else if (str_mode == "Twinkle Fade Random") {
    return (19);
  } else if (str_mode == "Sparkle") {
    return (20);
  } else if (str_mode == "Flash Sparkle") {
    return (21);
  } else if (str_mode == "Hyper Sparkle") {
    return (22);
  } else if (str_mode == "Strobe") {
    return (23);
  } else if (str_mode == "Strobe Rainbow") {
    return (24);
  } else if (str_mode == "Multi Strobe") {
    return (25);
  } else if (str_mode == "Blink Rainbow") {
    return (26);
  } else if (str_mode == "Chase White") {
    return (27);
  } else if (str_mode == "Chase Color") {
    return (28);
  } else if (str_mode == "Chase Random") {
    return (29);
  } else if (str_mode == "Chase Rainbow") {
    return (30);
  } else if (str_mode == "Chase Flash") {
    return (31);
  } else if (str_mode == "Chase Flash Random") {
    return (32);
  } else if (str_mode == "Chase Rainbow White") {
    return (33);
  } else if (str_mode == "Chase Blackout") {
    return (34);
  } else if (str_mode == "Chase Blackout Rainbow") {
    return (35);
  } else if (str_mode == "Color Sweep Random") {
    return (36);
  } else if (str_mode == "Running Color") {
    return (37);
  } else if (str_mode == "Running Red Blue") {
    return (38);
  } else if (str_mode == "Running Random") {
    return (39);
  } else if (str_mode == "Larson Scanner") {
    return (40);
  } else if (str_mode == "Comet") {
    return (41);
  } else if (str_mode == "Fireworks") {
    return (42);
  } else if (str_mode == "Fireworks Random") {
    return (43);
  } else if (str_mode == "Merry Christmas") {
    return (44);
  } else if (str_mode == "Fire Flicker") {
    return (45);
  } else if (str_mode == "Fire Flicker (soft)") {
    return (46);
  } else if (str_mode == "Dual Color Wipe In to Out") {
    return (47);
  } else if (str_mode == "Dual Color Wipe In to In") {
    return (48);
  } else if (str_mode == "Dual Color Wipe Out to Out") {
    return (49);
  } else if (str_mode == "Dual Color Wipe Out to In") {
    return (50);
  } else if (str_mode == "Police Normal") {
    return (51);
  } else if (str_mode == "Police 2 Bars") {
    return (52);
  } else if (str_mode == "Police 3 Blue 3 Red and one white Bar") {
    return (53);
  } else if (str_mode == "Police 2 Bars Strobe 3 Holt") {
    return (54);
  } else if (str_mode == "Music Visu") {
    return (200);
  } else {
    Serial.println("Get Modenum out of Reach");
    return (0);
  }
}

void pub_routine() {
  client.publish("/led_leiste/modes", String(ws2812fx.getModeName(e_mode)).c_str(), true);
  client.publish("/led_leiste/color", "255,0,0", true);
  client.publish("/led_leiste/speed", String(ws2812fx.getSpeed()).c_str(), true);
  client.publish("/led_leiste/bright", String(ws2812fx.getBrightness()).c_str(), true);
}


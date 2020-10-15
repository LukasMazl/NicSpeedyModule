# Vytváření rozšiřujícího modulu pro Python

Tento resporitař slouží k přípravě diplomové práce, která se zabívá porovnáním Pythno modulů
pro získání informací o NICs. Ve složce /module_tests jsou vytvořené zátěžové testy. Aktuální
výsledky těchto testu [jsou vidět zde.](./current_rest_results.txt)

##TODO LIST

- [ ] Zprovoznit modul ethtool pro testovani
- [ ] Vytvoření knihovny my_module_libs.c (například) pro extrahovaní logiky z my_module.c, tak aby my_module.c složil
pouze jako bridge mezi C a Python.
- [ ] Vytvořit python funkce pro lepší získání/manipulaci s daty, které příjdou z my_module.c
- [ ] Přejmenovat my_module.c!!!!! 
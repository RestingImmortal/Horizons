# Getting Started

This is a guide to get you up and running with a project using the Horizons Engine.

By the end, you will have the bare minimal foundations for launching a game with the engine,

## 1. Directories

You must manually create a folder next to the project root (Next to the executable) named `assets`. This is where your assets will be located.

## 2. Configuration

All configuration is handled through a file named `META.json`, found in the project root next to the executable.
As an example:
```json
{
  "title": "My Horizons Game",
  "log_level": "DEBUG"
}
```
The supported log levels are:

- TRACE
- DEBUG
- INFO
- WARNING
- ERROR
- CRITICAL

Any other log level will cause the logger to default to off. Setting logging entirely off is not recommended, but if you do so, you should mark it as `OFF`, fitting the format and maintaining ease of understanding.

## 3. Minimal Assets

The engine requires a few core data files to run: a Start file, which in turn references a Map file and a Ship file. An Affiliation file for the player is also required.

- Affiliation
- Start
- Map
- Ship

### 3.1. Affiliations

Create a file within the `assets` directory by any name you wish, though I recommend titling it `player.affiliation.json` or `player.affiliation.xml`. Do note that you can move these into a subdirectory of `assets` if you wish, such as `assets/factions`.

Depending on the filetype you choose, the file should look like the following:
```json
{
  "name": "Player",
  "relations": []
}
```
```xml
<AffiliationData>
    <name>Player</name>
</AffiliationData>
```
Note that the XML doesn't specify any relations.

### 3.2. Starts

Currently, only one start is supported, that being the `Multi-start unsupported still. Harass Luna if this is limiting you` start.

Similarly to the affiliation, make a file in `assets` or a subdirectory of it, naming it what you wish, ending it with `.start.json` or `.start.xml`.

It should look like this:
```json
{
  "name": "Multi-start unsupported still. Harass Luna if this is limiting you",
  "initial_map": "my_first_map",
  "player": {
    "ship_type": "my_first_ship",
    "x": 0.0,
    "y": 0.0
  }
}
```
```xml
<StartData>
    <name>Multi-start unsupported still. Harass Luna if this is limiting you</name>
    <initial_map>my_first_map</initial_map>
    <player>
        <ship_type>my_first_ship</ship_type>
        <x>0.0</x>
        <y>0.0</y>
    </player>
</StartData>
```

### 3.3. Maps

Same as Start data, create your asset file, with the extension of either `.map.json` or `.map.xml`.

```json
{
  "meta": {
    "name": "my_first_map"
  },
  "backgrounds": [],
  "ships": [],
  "objects": []
}
```
```xml
<MapData>
    <meta>
        <name>my_first_map</name>
    </meta>
</MapData>
```

### 3.4. Ships

Create your asset file. At this time, **the filename matters**. If you followed this guide so far, the name should be `my_first_ship.ship.json` or `my_first_ship.ship.xml`.

```json
{
  "texture": "",
  "max_speed": 0.0,
  "radius": 0,
  "weapons": [],
  "engines": []
}
```
```xml
<ShipData>
    <texture></texture>
    <max_speed>0.0</max_speed>
    <radius>0</radius>
</ShipData>
```

## 4. Running the game

At this stage, all you have to do are [Compile the game](../README.md), then run the resulting `game` executable.

This is your beginning for the game you wish to bring to life.

## 5. Next Steps

The things you should work on after this point depend entirely on you, but really, you should get things looking nice and actually functioning first. Here are a few examples:

- Give your ship a texture so you see more than a magenta box.
- Make your ship move by giving it an engine, specified through an engine asset type and raising the ship's max speed.
- Enhance the map with a background and an object.
- Create an enemy faction with hostile relations to the player, spawning them in through the map.
- Create a weapon asset and add it to your ship so you can battle with the enemy.

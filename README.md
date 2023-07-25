# Grape
### EOS Implementation on Unreal Engine using OnlineSubsystem

This project is the implementation of Epic Games Online Services via Unreal Engine.
The purpose of this project is to have all the integrations with EOS in a single project so it can be re-used.

This implementation is totally focused on Dedicated Servers.

# Current support

- Dedicated Server Session ✅
  - Session Creation ✅
  - Session Finding ✅
  - Session Joining ✅
- Matchmaking ✅
- Stats ✅
   - Query Stats ✅
   - Update Stats ✅
- Achievements 🚧
- Leaderboards 🚧
- Player Data Storage ❌
- Anti-Cheat ❌
- Title Storage ❌
- Lobbies ❌
- Friends ❌
- Parties ❌
- Presence ❌
- Steam Crossplay ❌

# Config

- Make sure to have the right artifacts configured and you got one for the client and one for the server
- Make sure you have two clients on your Epic Games Portal one for the client and one for the server
- Change your artifacts every time you package your server or your client
- Include the OnlineSubsystem modules on your Project.build.cs
  
 ```C# 
 PublicDependencyModuleNames.AddRange(new string[] { "Core", "OnlineSubsystem", "OnlineSubsystemEOS", "OnlineSubsystemUtils", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
```

- Include a Login button to avoid crashing problems
- Always remember that Stats names should always be Uppercase or it can lead to Epic Games API not recognizing your key value

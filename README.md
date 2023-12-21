# Unreal Engine 5.2 EOS OnlineSubsystem
### EOS Implementation on Unreal Engine using OnlineSubsystem

This project is an implementation template of Epic Games Online Services via Unreal Engine.
The purpose of this project is to have re-usable integrations with EOS.

All implementations are made for Dedicated Servers but can also be used for Listen Servers with minimal change.

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

- Make sure to have the artifacts configured for the client and server
- Also make sure you have two clients on your Epic Games Portal matching the last step
- Select the right artifacts every time you package your project (Client/Server)
- Include OnlineSubsystem modules on your Project.build.cs
  
 ```C# 
 PublicDependencyModuleNames.AddRange(new string[] { "Core", "OnlineSubsystem", "OnlineSubsystemEOS", "OnlineSubsystemUtils", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
```

- Include a manual Login button to avoid crashes
- Keep in mind that Stats names should always be Uppercase or it can lead to Epic Games API not finding your key value

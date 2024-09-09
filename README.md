# Fido2.0 Server using MFC (Windows Application)

This project is a Windows application built using Microsoft Foundation Classes (MFC) that acts as a Fido2.0 server. While still under development, the goal of the project is to implement a server that supports Fido2.0 authentication, facilitating secure, password-less authentication for web applications and services.

## Project Overview

Fido2.0 is an industry standard that enables strong, public key cryptography-based authentication. This project aims to implement the server-side logic that interacts with client devices to securely register and authenticate users using hardware-based tokens (such as USB security keys) or biometric methods (such as fingerprint readers).

### Features (Planned)
- **User Registration:** Handles the initial setup of Fido2.0 devices for users, allowing them to register a security key or biometric authenticator.
- **Authentication:** Validates users based on their registered Fido2.0 devices.
- **Public-Key Cryptography:** Implements strong cryptographic protocols to ensure secure communication between clients and the server.
- **MFC-based GUI:** Provides a Windows-native graphical user interface for managing server operations and user accounts.

### Current Progress
- Basic structure of the MFC Windows application is in place.
- Partial implementation of the Fido2.0 registration flow.
- Basic GUI for starting and stopping the server.

### To-do
- Complete Fido2.0 registration flow.
- Implement full Fido2.0 authentication logic.
- Integrate secure communication protocols (e.g., HTTPS).
- Extend GUI for user management and device management.
- Add error handling and logging.

## Installation

1. **Clone the repository:**

    ```bash
    git clone https://github.com/your-repo/fido2-mfc-server.git
    ```

2. **Set up Visual Studio:**
   Open the project in Visual Studio (recommended version 2019 or later) and ensure that MFC is installed as part of your Visual Studio environment.

3. **Build the Project:**
   Select your build configuration (e.g., Debug or Release) and build the project using `Build -> Build Solution` in Visual Studio.

4. **Run the Application:**
   Once built, you can run the application directly from Visual Studio (`Debug -> Start Debugging`), or execute the binary from the `bin/` folder.

## Usage

- **Start/Stop Server:** Use the GUI controls to start or stop the Fido2.0 server.
- **Register Users:** (Planned) Initiate user registration by connecting a Fido2.0 device, such as a USB security key.
- **Authenticate Users:** (Planned) Authenticate a user by verifying their registered Fido2.0 device.

## Contribution

As this project is still under development, contributions and feedback are welcome. Here's how you can contribute:

1. **Fork the repository.**
2. **Create a branch for your feature:** (`git checkout -b feature-name`)
3. **Commit your changes:** (`git commit -m 'Add feature'`)
4. **Push to the branch:** (`git push origin feature-name`)
5. **Submit a pull request.**

## Future Enhancements

- **Fido2.0 Extensions:** Implement additional Fido2.0 features such as user verification, PIN support, and attestation.
- **Cross-Platform Compatibility:** Investigate porting the server to other platforms such as Linux.
- **Database Integration:** Store user data and credentials in a secure database.
- **Scalability:** Add features to scale the server for enterprise-level use cases.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

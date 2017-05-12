//
//  LoginViewController.swift
//  CBB
//
//  Created by Erich Chu on 4/3/17.
//  Copyright © 2017 Erich Chu. All rights reserved.
//

import Foundation
import UIKit
import Alamofire

class LoginViewController: UIViewController {
    @IBOutlet weak var uname: UITextField!
    @IBOutlet weak var password: UITextField!
    @IBOutlet weak var headerText: UILabel!
    @IBOutlet weak var errorMsg: UILabel!
    @IBOutlet weak var loginBtn: UIButton!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        initializeViewStyles()
        uname.addTarget(self, action: #selector(verifyUserField(_:)), for: .editingDidEnd)
        password.addTarget(self, action: #selector(verifyPWField(_:)), for: .editingDidEnd)
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    func initializeViewStyles() {
        let defaultBorderColor : UIColor = UIColor( red: 169/255, green: 169/255, blue:169/255, alpha: 1.0 )
        uname.layer.borderColor = defaultBorderColor.cgColor
        uname.layer.borderWidth = 1.0
        password.layer.borderColor = defaultBorderColor.cgColor
        password.layer.borderWidth = 1.0
    }
    
    func sha256(string: String) -> String? {
        guard let messageData = string.data(using:String.Encoding.utf8) else { return nil; }
        var digestData = Data(count: Int(CC_SHA256_DIGEST_LENGTH))
        
        _ = digestData.withUnsafeMutableBytes {digestBytes in
            messageData.withUnsafeBytes {messageBytes in
                CC_SHA256(messageBytes, CC_LONG(messageData.count), digestBytes)
            }
        }
        return digestData.map { String(format: "%02hhx", $0) }.joined()
    }
    
    func verifyUserField(_ tf: UITextField) -> Bool {
        let trimmedTF = tf.text?.trimmingCharacters(in: .whitespaces)
        if (trimmedTF!.isEmpty) {
            errorMsg.text = "Username can't be empty"
            return false
        } else {
            errorMsg.text = ""
        }
        return true
    }
    
    func verifyPWField(_ tf: UITextField) -> Bool {
        if (tf.text!.isEmpty) {
            errorMsg.text = "Password can't be empty"
            return false
        } else {
            errorMsg.text = ""
        }
        return true
    }
    
    func verifyCredentials() {
        let finalUser = uname.text?.trimmingCharacters(in: .whitespaces)
        let hashedPW = sha256(string: password.text!)
        let finalPW = hashedPW
        let parameters: Parameters = [
            "username": finalUser,
            "password": finalPW,
        ]
        
        // create post request
        let url = "https://code.engineering.nyu.edu:8080/user/login"
        Alamofire.request(url, method: .post, parameters: parameters, encoding: JSONEncoding.default).responseJSON { response in
            if (response.response?.statusCode == nil) {
                self.errorMsg.text = "Couldn't connect to server."
            }
            else if (response.response?.statusCode==400) {
                self.errorMsg.text = "Invalid login credentials. Try again."
            }
            else if (response.response?.statusCode==200) {
                self.moveToNextView()
            }
        }
    }
    
    func moveToNextView() {
        let mainStoryboard:UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
        let newFrontViewController = mainStoryboard.instantiateViewController(withIdentifier: "SWRevealViewController") as! SWRevealViewController
        self.present(newFrontViewController, animated: true, completion: nil)

    }
    
    @IBAction func loginToCBB(_ sender: UIButton) {
        if(verifyUserField(uname) && verifyPWField(password)){
            verifyCredentials()
        }
    }
}

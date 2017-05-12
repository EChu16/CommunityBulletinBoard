//
//  CBBViewController.swift
//  CBB
//
//  Created by Erich Chu on 4/22/17.
//  Copyright © 2017 Erich Chu. All rights reserved.
//

import UIKit
import Alamofire
import SwiftyJSON

class CBBViewController: UIViewController {

    @IBOutlet weak var menuButton: UIBarButtonItem!
    @IBOutlet weak var u_num: UILabel!
    @IBOutlet weak var c_name: UILabel!
    @IBOutlet weak var c_desc: UITextView!
    @IBOutlet weak var actionBtn: UIButton!
    @IBOutlet weak var viewBtn: UIButton!
    
    var community_name = ""
    var community_desc = ""
    var community_oid = ""
    var user_name = ""
    var user_id = ""
    
    override func viewDidLoad() {
        super.viewDidLoad()
        setupMenu()
        self.view.layer.borderWidth = 10
        self.view.layer.borderColor = UIColor.black.cgColor
        self.view.backgroundColor = UIColor(patternImage: #imageLiteral(resourceName: "cork-back3"))
        retrieveBoardInfo()
        isUserInCommunity()
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    func setupMenu() {
        if (revealViewController() != nil) {
            menuButton.target = revealViewController()
            menuButton.action = #selector(SWRevealViewController.revealToggle(_:))
            revealViewController().rearViewRevealWidth = 275
            
            view.addGestureRecognizer(self.revealViewController().panGestureRecognizer())
        }
        customNavBar()
        customTextFieldStyle()
        self.c_name.text! = self.community_name
    }
    
    func customNavBar() {
        let navImg = #imageLiteral(resourceName: "nav-bar")
        
        self.navigationController?.navigationBar.setBackgroundImage(navImg, for: .default)
        
        navigationController?.navigationBar.tintColor = UIColor(colorLiteralRed: 1.0, green: 1.0, blue: 1.0, alpha: 1.0)
        navigationController?.navigationBar.titleTextAttributes = [NSForegroundColorAttributeName: UIColor.white, NSFontAttributeName: UIFont(name: "Aleo-Bold", size: 20.0)]

    }
    
    func customTextFieldStyle() {
        c_desc.layer.borderColor = UIColor(colorLiteralRed: 204.0/255.0, green: 204.0/255.0, blue: 204.0/255.0, alpha: 1.0).cgColor
        c_desc.layer.borderWidth = 1.0
        c_desc.layer.cornerRadius = 5.0
        c_name.layer.borderColor = UIColor(colorLiteralRed: 204.0/255.0, green: 204.0/255.0, blue: 204.0/255.0, alpha: 1.0).cgColor
        c_name.layer.borderWidth = 1.0
        c_name.layer.cornerRadius = 5.0
    }
    
    func setupButton() {
        if(actionBtn.currentTitle == "Join") {
            viewBtn.isHidden = true
            viewBtn.isEnabled = false
        } else {
            viewBtn.isHidden = false
            viewBtn.isEnabled = true
        }
        actionBtn.addTarget(self, action:#selector(sendActionRequest(_:)), for: .touchUpInside)
    }
    
    override func viewWillAppear(_ animated: Bool) {
        self.navigationItem.title = "Community"
    }
    
    func isUserInCommunity() {
        // create post request
        let parameters: Parameters = [
            "community_name": community_name,
            ]
        let url = "https://code.engineering.nyu.edu:8080/cbb/hasuser"
        Alamofire.request(url, method: .post, parameters: parameters, encoding: JSONEncoding.default).responseJSON { response in
            if (response.response?.statusCode == nil) {
                print("Couldn't connect to server.")
            }
            else if (response.response?.statusCode==400) {
                print ("hit")
                debugPrint(String(data: response.data!, encoding: String.Encoding.utf8))
                print(response)
            }
            else if (response.response?.statusCode==200) {
                let actionName = String(data: response.data!, encoding: String.Encoding.utf8)
                self.actionBtn.setTitle(actionName, for: .normal)
                self.actionBtn.titleLabel?.font = UIFont(name: "Aleo-Bold", size: 20.0)
                self.setupButton()
            }
        }
    }
    
    func retrieveBoardInfo() {
        // create post request
        let parameters: Parameters = [
            "community_name": community_name,
            ]
        let url = "https://code.engineering.nyu.edu:8080/cbb/view"
        Alamofire.request(url, method: .post, parameters: parameters, encoding: JSONEncoding.default).responseJSON { response in
            if (response.response?.statusCode == nil) {
                print("Couldn't connect to server.")
            }
            else if (response.response?.statusCode==400) {
                print ("hit")
                debugPrint(String(data: response.data!, encoding: String.Encoding.utf8))
                print(response)
            }
            else if (response.response?.statusCode==200) {
                guard let object = response.result.value else {
                    print("Error")
                    return
                }
                let json = JSON(object)
                if let board = json.array {
                    self.community_desc = board[0]["description"].string!
                    self.community_oid = (board[0]["owner_id"].dictionary?["$oid"]?.string!)!
                }
                self.c_desc.text! = self.community_desc
                if (self.c_desc.text!) == "" {
                    self.c_desc.text! = "No Description"
                }
            }
        }
    }

    @IBAction func sendActionRequest(_ sender: UIButton) {
        if (sender.titleLabel?.text == "Join") {
            sendJoinRequest()
        } else if (sender.titleLabel?.text == "Leave") {
            sendLeaveRequest()
        }
    }
    
    @IBAction func viewPosts(_ sender: UIButton) {
        if (sender.titleLabel?.text == "View Posts") {
            goToCommunity()
        }
    }
    
    @IBAction func viewUserList(_ sender: UIButton) {
        if (sender.titleLabel?.text == "List Users") {
            goToUserList()
        }
    }
    
    func sendJoinRequest() {
        // create post request
        let parameters: Parameters = [
            "community_name": community_name,
            ]
        let url = "https://code.engineering.nyu.edu:8080/cbb/join"
        Alamofire.request(url, method: .post, parameters: parameters, encoding: JSONEncoding.default).responseJSON { response in
            if (response.response?.statusCode == nil) {
                print("Couldn't connect to server.")
            }
            else if (response.response?.statusCode==400) {
                print ("hit")
                debugPrint(String(data: response.data!, encoding: String.Encoding.utf8))
                print(response)
            }
            else if (response.response?.statusCode==200) {
                self.goToCommunity()
            }
        }
    }
    
    func sendLeaveRequest() {
        // create post request
        let parameters: Parameters = [
            "community_name": community_name,
            ]
        let url = "https://code.engineering.nyu.edu:8080/cbb/leave"
        Alamofire.request(url, method: .post, parameters: parameters, encoding: JSONEncoding.default).responseJSON { response in
            if (response.response?.statusCode == nil) {
                print("Couldn't connect to server.")
            }
            else if (response.response?.statusCode==400) {
                print ("hit")
                debugPrint(String(data: response.data!, encoding: String.Encoding.utf8))
                print(response)
            }
            else if (response.response?.statusCode==200) {
                let revealViewController:SWRevealViewController = self.revealViewController()
                let mainStoryboard:UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
                let desController = mainStoryboard.instantiateViewController(withIdentifier: "CommunityViewController") as! CommunityViewController
                desController.user_name = self.user_name
                desController.user_id = self.user_id
                let newFrontViewController = UINavigationController.init(rootViewController:desController)
                revealViewController.pushFrontViewController(newFrontViewController, animated: true)
            }
        }
    }
    
    func goToCommunity() {
        let revealViewController:SWRevealViewController = self.revealViewController()
        let mainStoryboard:UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
        let desController = mainStoryboard.instantiateViewController(withIdentifier: "PostsViewController") as! PostsViewController
        desController.community_name = self.community_name
        desController.user_name = self.user_name
        desController.user_id = self.user_id
        desController.community_oid = self.community_oid
        let newFrontViewController = UINavigationController.init(rootViewController:desController)
        revealViewController.pushFrontViewController(newFrontViewController, animated: true)
    }
    
    func goToUserList() {
        let revealViewController:SWRevealViewController = self.revealViewController()
        let mainStoryboard:UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
        let desController = mainStoryboard.instantiateViewController(withIdentifier: "UserListViewController") as! UserListViewController
        desController.user_name = self.user_name
        desController.user_id = self.user_id
        desController.community_name = self.community_name
        desController.community_desc = self.community_desc
        desController.community_oid = self.community_oid
        let newFrontViewController = UINavigationController.init(rootViewController:desController)
        revealViewController.pushFrontViewController(newFrontViewController, animated: true)
    }
}

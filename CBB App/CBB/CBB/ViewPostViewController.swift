//
//  ViewPostViewController.swift
//  CBB
//
//  Created by Erich Chu on 4/23/17.
//  Copyright © 2017 Erich Chu. All rights reserved.
//

import UIKit
import Alamofire
import SwiftyJSON

class ViewPostViewController: UIViewController {
    var owner_id = ""
    var post_id = ""
    var community_name = ""
    var postdate = ""
    var posttime = ""
    var pTitle = ""
    var pDesc = ""
    var name = ""
    var community_oid = ""
    
    var user_name = ""
    var user_id = ""
    
    @IBOutlet weak var poster_name: UILabel!
    @IBOutlet weak var post_date: UILabel!
    @IBOutlet weak var post_time: UILabel!
    @IBOutlet weak var postTitle: UILabel!
    @IBOutlet weak var postDesc: UITextView!
    @IBOutlet weak var errorMsg: UILabel!
    
    @IBOutlet weak var editButton: UIButton!
    @IBOutlet weak var deleteButton: UIButton!
    @IBOutlet weak var submitButton: UIButton!

    @IBOutlet weak var menuButton: UIBarButtonItem!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        setupMenu()
        setupLabels()
        displayToolButtons()
        self.view.layer.borderWidth = 10
        self.view.layer.borderColor = UIColor.black.cgColor
        self.view.backgroundColor = UIColor(patternImage: #imageLiteral(resourceName: "cork-back3"))
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    override func viewWillAppear(_ animated: Bool) {
        self.navigationItem.title = "Post"
    }
    
    func setupMenu() {
        if (revealViewController() != nil) {
            menuButton.target = revealViewController()
            menuButton.action = #selector(SWRevealViewController.revealToggle(_:))
            revealViewController().rearViewRevealWidth = 275
            
            view.addGestureRecognizer(self.revealViewController().panGestureRecognizer())
        }
        customNavBar()
    }
    
    func customNavBar() {
        let navImg = #imageLiteral(resourceName: "nav-bar")
        
        self.navigationController?.navigationBar.setBackgroundImage(navImg, for: .default)
        
        navigationController?.navigationBar.tintColor = UIColor(colorLiteralRed: 1.0, green: 1.0, blue: 1.0, alpha: 1.0)
        navigationController?.navigationBar.titleTextAttributes = [NSForegroundColorAttributeName: UIColor.white, NSFontAttributeName: UIFont(name: "Aleo-Bold", size: 20.0)]

    }
    
    func displayToolButtons() {
        if(owner_id == user_id) {
            self.editButton.isHidden = false
            self.editButton.isEnabled = true
            self.deleteButton.isHidden = false
            self.deleteButton.isEnabled = true
        }
    }
    
    @IBAction func sendBackToCommunity(_ sender: UIBarButtonItem) {
        self.goBackToCommunity()
    }
    
    func goBackToCommunity() {
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
    
    @IBAction func startEditPost(_ sender: UIButton) {
        if (sender.titleLabel?.text == "Edit") {
            self.editPost()
        }
    }
    
    @IBAction func deletePost(_ sender: UIButton) {
        if (sender.titleLabel?.text == "Delete") {
            self.deletePost()
        }
    }
    
    @IBAction func commentOnPost(_ sender: UIButton) {
        if (sender.titleLabel?.text == "Comment") {
            self.sendToCommentPage()
        } else if (sender.titleLabel?.text == "Finish") {
            self.sendEditRequest()
        }
    }
    
    func sendEditRequest() {
        let finalPostDesc = postDesc.text?.trimmingCharacters(in: .whitespaces)
        if(finalPostDesc!.isEmpty) {
            errorMsg.text = "Post description can't be empty"
        } else {
            let parameters: Parameters = [
                "post_id": self.post_id,
                "post_title": self.pTitle,
                "post_body": finalPostDesc
            ]
            
            // create post request
            let url = "https://code.engineering.nyu.edu:8080/posts/edit"
            Alamofire.request(url, method: .post, parameters: parameters, encoding: JSONEncoding.default).responseJSON { response in
                if (response.response?.statusCode == nil) {
                    self.errorMsg.text = "Couldn't connect to server."
                }
                else if (response.response?.statusCode==400) {
                    self.errorMsg.text = String(data: response.data!, encoding: String.Encoding.utf8)
                }
                else if (response.response?.statusCode==200) {
                    self.errorMsg.text! = ""
                    self.postDesc.isEditable = false
                    self.editButton.isHidden = false
                    self.editButton.isEnabled = true
                    self.deleteButton.isHidden = false
                    self.deleteButton.isEnabled = true
                    self.submitButton.setTitle("Comment", for: .normal)
                    
                }
            }
        }
    }
    
    func editPost() {
        self.postDesc.isEditable = true
        self.submitButton.setTitle("Finish", for: .normal)
    }
    
    func deletePost() {
        let parameters: Parameters = [
            "post_id": self.post_id
            ]
        
        // create post request
        let url = "https://code.engineering.nyu.edu:8080/posts/delete"
        Alamofire.request(url, method: .post, parameters: parameters, encoding: JSONEncoding.default).responseJSON { response in
            if (response.response?.statusCode == nil) {
                self.errorMsg.text = "Couldn't connect to server."
            }
            else if (response.response?.statusCode==400) {
                self.errorMsg.text = String(data: response.data!, encoding: String.Encoding.utf8)
            }
            else if (response.response?.statusCode==200) {
                self.goBackToCommunity()
            }
        }
    }
    
    func sendToCommentPage() {
        let revealViewController:SWRevealViewController = self.revealViewController()
        let mainStoryboard:UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
        let desController = mainStoryboard.instantiateViewController(withIdentifier: "CommentsViewController") as! CommentsViewController
        
        desController.user_name = self.user_name
        desController.user_id = self.user_id
        desController.post_id = self.post_id
        desController.post_name = self.pTitle
        desController.poster_name = self.name
        desController.post_title = self.pTitle
        desController.post_desc = self.pDesc
        desController.post_date = self.postdate
        desController.post_time = self.posttime
        desController.post_oid = self.owner_id
        desController.community_name = self.community_name
        desController.community_oid = self.community_oid
        
        let newFrontViewController = UINavigationController.init(rootViewController:desController)
        revealViewController.pushFrontViewController(newFrontViewController, animated: true)
    }
    
    func setupLabels() {
        self.post_date.text! = self.postdate
        self.post_time.text! = self.posttime
        self.postTitle.text! = self.pTitle
        self.postDesc.text! = self.pDesc
        self.poster_name.text! = self.name
        
        postTitle.layer.borderColor = UIColor(colorLiteralRed: 204.0/255.0, green: 204.0/255.0, blue: 204.0/255.0, alpha: 1.0).cgColor
        postTitle.layer.borderWidth = 1.0
        postTitle.layer.cornerRadius = 5.0
        
        postDesc.layer.borderColor = UIColor(colorLiteralRed: 204.0/255.0, green: 204.0/255.0, blue: 204.0/255.0, alpha: 1.0).cgColor
        postDesc.layer.borderWidth = 1.0
        postDesc.layer.cornerRadius = 5.0

        
    }
}

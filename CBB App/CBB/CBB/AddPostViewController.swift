//
//  AddPostViewController.swift
//  CBB
//
//  Created by Erich Chu on 4/22/17.
//  Copyright © 2017 Erich Chu. All rights reserved.
//

import UIKit
import Alamofire
import SwiftyJSON

class AddPostViewController: UIViewController {
    
    var community_name = ""
    var user_name = ""
    var user_id = ""
    var community_oid = ""
    
    @IBOutlet weak var menuButton: UIBarButtonItem!
    @IBOutlet weak var postTitle: UITextField!
    @IBOutlet weak var postDesc: UITextView!
    @IBOutlet weak var errorMsg: UILabel!
    @IBOutlet weak var submitBtn: UIButton!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        setupMenu()
        self.view.layer.borderWidth = 10
        self.view.layer.borderColor = UIColor.black.cgColor
        self.view.backgroundColor = UIColor(patternImage: #imageLiteral(resourceName: "cork-back3"))
    }
    
    @IBAction func backToPostsView(_ sender: UIBarButtonItem) {
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
    
    func setupMenu() {
        if (revealViewController() != nil) {
            menuButton.target = revealViewController()
            menuButton.action = #selector(SWRevealViewController.revealToggle(_:))
            revealViewController().rearViewRevealWidth = 275
            
            view.addGestureRecognizer(self.revealViewController().panGestureRecognizer())
        }
        customNavBar()
        setupTextField()
    }
    
    func customNavBar() {
        let navImg = #imageLiteral(resourceName: "nav-bar")
        
        self.navigationController?.navigationBar.setBackgroundImage(navImg, for: .default)
        
        navigationController?.navigationBar.tintColor = UIColor(colorLiteralRed: 1.0, green: 1.0, blue: 1.0, alpha: 1.0)
        navigationController?.navigationBar.titleTextAttributes = [NSForegroundColorAttributeName: UIColor.white, NSFontAttributeName: UIFont(name: "Aleo-Bold", size: 20.0)]

    }
    
    func setupTextField() {
        postDesc.text = "Post description"
        postDesc.textColor = UIColor.lightGray
        
        postDesc.layer.borderColor = UIColor(colorLiteralRed: 204.0/255.0, green: 204.0/255.0, blue: 204.0/255.0, alpha: 1.0).cgColor
        postDesc.layer.borderWidth = 1.0
        postDesc.layer.cornerRadius = 5.0
    }
    
    func textViewDidBeginEditing(_ textView: UITextView) {
        if textView.textColor == UIColor.lightGray {
            textView.text = nil
            textView.textColor = UIColor.black
        }
    }
    
    func textViewDidEndEditing(_ textView: UITextView) {
        if textView.text.isEmpty {
            textView.text = "Post description"
            textView.textColor = UIColor.lightGray
        }
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    override func viewWillAppear(_ animated: Bool) {
        self.navigationItem.title = "Add Post"
    }
    
    @IBAction func addPostRequest(_ sender: UIButton) {
        let finalPostTitle = postTitle.text?.trimmingCharacters(in: .whitespaces)
        let finalPostDesc = postDesc.text?.trimmingCharacters(in: .whitespaces)
        if(finalPostTitle!.isEmpty) {
            errorMsg.text = "Post title can't be empty"
        }
        else if(finalPostDesc!.isEmpty) {
            errorMsg.text = "Post description can't be empty"
        }
        else {
            sendPostRequest(title: finalPostTitle!, desc: finalPostDesc!)
        }
    }
    
    func sendPostRequest(title: String, desc: String) {
        let parameters: Parameters = [
            "post_title": title,
            "post_body": desc,
            "community_name": self.community_name
        ]
        // create post request
        let url = "https://code.engineering.nyu.edu:8080/posts/new"
        Alamofire.request(url, method: .post, parameters: parameters, encoding: JSONEncoding.default).responseJSON { response in
            if (response.response?.statusCode == nil) {
                self.errorMsg.text = "Couldn't connect to server."
            }
            else if (response.response?.statusCode==400) {
                self.errorMsg.text = String(data: response.data!, encoding: String.Encoding.utf8)
            }
            else if (response.response?.statusCode==200) {
                self.moveBackToCommunityPostsView()
            }
        }
    }
    
    func moveBackToCommunityPostsView() {
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
}

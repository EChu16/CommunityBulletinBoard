//
//  AddBoardViewController.swift
//  CBB
//
//  Created by Erich Chu on 4/21/17.
//  Copyright © 2017 Erich Chu. All rights reserved.
//

import UIKit
import Alamofire

class AddBoardViewController: UIViewController, UITextViewDelegate {
    
    @IBOutlet weak var errorMsg: UILabel!
    @IBOutlet weak var menuButton: UIBarButtonItem!
    @IBOutlet weak var boardName: UITextField!
    @IBOutlet weak var boardDesc: UITextView!
    
    var user_name = ""
    var user_id = ""
    
    override func viewDidLoad() {
        super.viewDidLoad()
        setupMenu()
        self.view.layer.borderWidth = 10
        self.view.layer.borderColor = UIColor.black.cgColor
        self.view.backgroundColor = UIColor(patternImage: #imageLiteral(resourceName: "cork-back3"))
    }
    
    override func viewWillAppear(_ animated: Bool) {
        self.navigationItem.title = "Add Community"
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    
    @IBAction func backToCommunityView(_ sender: UIBarButtonItem) {
        self.moveBackToCommunityView()
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
        boardDesc.text = "Community description"
        boardDesc.textColor = UIColor.lightGray
        
        boardDesc.layer.borderColor = UIColor(colorLiteralRed: 204.0/255.0, green: 204.0/255.0, blue: 204.0/255.0, alpha: 1.0).cgColor
        boardDesc.layer.borderWidth = 1.0
        boardDesc.layer.cornerRadius = 5.0
    }
    
    func textViewDidBeginEditing(_ textView: UITextView) {
        if textView.textColor == UIColor.lightGray {
            textView.text = nil
            textView.textColor = UIColor.black
        }
    }
    
    func textViewDidEndEditing(_ textView: UITextView) {
        if textView.text.isEmpty {
            textView.text = "Community description"
            textView.textColor = UIColor.lightGray
        }
    }
    
    @IBAction func submitBoard(_ sender: UIButton) {
        let finalBoardName = boardName.text?.trimmingCharacters(in: .whitespaces)
        let finalBoardDesc = boardDesc.text?.trimmingCharacters(in: .whitespaces)
        if(finalBoardName!.isEmpty) {
            errorMsg.text = "Board name can't be empty"
        } else {
            let parameters: Parameters = [
                "community_name": finalBoardName,
                "community_desc": finalBoardDesc
            ]
            // create post request
            let url = "https://code.engineering.nyu.edu:8080/cbb/new"
            Alamofire.request(url, method: .post, parameters: parameters, encoding: JSONEncoding.default).responseJSON { response in
                if (response.response?.statusCode == nil) {
                    self.errorMsg.text = "Couldn't connect to server."
                }
                else if (response.response?.statusCode==400) {
                    self.errorMsg.text = "Existing community found. Try another."
                }
                else if (response.response?.statusCode==200) {
                    self.moveBackToCommunityView()
                }
            }
        }
    }
    
    func moveBackToCommunityView() {
        let revealViewController:SWRevealViewController = self.revealViewController()
        let mainStoryboard:UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
        let desController = mainStoryboard.instantiateViewController(withIdentifier: "CommunityViewController") as! CommunityViewController
        desController.user_id = self.user_id
        desController.user_name = self.user_name
        let newFrontViewController = UINavigationController.init(rootViewController:desController)
        revealViewController.pushFrontViewController(newFrontViewController, animated: true)
    }
}
